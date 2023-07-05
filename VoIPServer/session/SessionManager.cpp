#include <iostream>
#include <sstream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SessionManager.h"
#include "TelephonyManager.h"
#include "AccountManager.h"

SessionManager* SessionManager::instance = nullptr;
SOCKET serverSocket;
bool isRunning = true;

SessionManager::SessionManager() {
	contactNum = 0;

	telephonyManager = TelephonyManager::getInstance();
	accountManager = AccountManager::getInstance();
}

SessionManager* SessionManager::getInstance() {
	if (instance == nullptr) {
		instance = new SessionManager();
	}
	return instance;
}

void SessionManager::releaseInstance() {
	if (instance != nullptr) {
		instance->release();
		delete instance;
		instance = nullptr;
	}
}

void SessionManager::init() {
	telephonyManager->setSessionControl(this);
	accountManager->setSessionControl(this);

	std::thread sessionThread(&SessionManager::openSocket, instance);
	sessionThread.join();
}

void SessionManager::release() {
	std::cout << "SessionManager release" << std::endl;
	for (const auto& client : clientMap) {
		shutdown(client.second, SD_SEND);
	}
	isRunning = false;
	closesocket(serverSocket);

	telephonyManager->setSessionControl(nullptr);
	accountManager->setSessionControl(nullptr);
}

void SessionManager::openSocket() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock." << std::endl;
		return;
	}

	serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == -1) {
		std::cerr << "Failed to create socket." << std::endl;
		return;
	}

	SOCKADDR_IN serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverPort);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		/* data */
		std::cerr << "Failed to bind." << std::endl;
		closesocket(serverSocket);
		WSACleanup();
		return;
	};

	if (listen(serverSocket, MAX_CLIENTS) == -1) {
		std::cerr << "Failed to listen." << std::endl;
	}

	std::cout << "Waiting for client..." << std::endl;
	while (isRunning) {
		SOCKADDR_IN clientAddress{};
		int clientAddressLengh = sizeof(clientAddress);
		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressLengh);
		if (clientSocket == -1) {
			std::cerr << "Failed to accept client connection." << std::endl;
			continue;
		}

		// Ŭ���̾�Ʈ���� ����� ���� ������� ó��
		std::thread clientThread(&SessionManager::HandleClient, instance, clientSocket);
		clientThread.detach();
	}

	// ���� ���� �ݱ�
	closesocket(serverSocket);
	WSACleanup();
}

void SessionManager::HandleClient(int clientSocket) {
	// Ŭ���̾�Ʈ���� ��� ó��
	// ��: �޽��� �ۼ���, ������ ó�� ��

	std::string displayName = GetClientName(clientSocket);

	std::cout << "Connected to client! " << displayName << std::endl;

	std::string contactId;
	char buffer[PACKET_SIZE];
	while (true) {
		memset(buffer, 0, sizeof(buffer));

		// Ŭ���̾�Ʈ�κ��� �޼��� ����
		SSIZE_T bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (bytesRead == -1) {
			std::cerr << "Failed to receive message" << displayName << std::endl;
			break;
		}
		if (bytesRead == 0) {
			std::cout << "disconnected to client." << displayName << std::endl;
			break;
		}

		// listener test
		std::string msg(buffer);
		if (msg.find("Login") != std::string::npos) {
			contactId = ("CONTACT_" + std::to_string(contactNum++));
			clientMap.insert({ contactId, clientSocket });
			accountManager->handleLogin_(contactId.c_str());
		}
		else if (msg.find("startOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string to(tokens.back());
			if (clientMap.count(to) == 0) {
				sendData("onFailedOutgoingCall,unreachable", contactId);
				continue;
			}
			telephonyManager->handleOutgoingCall(contactId, to);
		}
		else if (msg.find("answerCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			telephonyManager->handleAnswer(tokens.back(), contactId);
		}
		else if (msg.find("rejectCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string cause = tokens[tokens.size() - 2];
			telephonyManager->handleReject(tokens.back(), cause, contactId);
		}
		else if (msg.find("disconnectCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			telephonyManager->handleDisconnect(tokens.back());
		}

		std::cout << "Received message from client" << displayName << ": " << buffer << std::endl;

		//-------------------------------------------------------------
		// JSON payload parser
		Json::Reader jsonReader;
		Json::Value jsonData;
		if (jsonReader.parse(msg, jsonData) == true) {
			// received data parsed as JSON data			
			int msgId = std::stoi(jsonData["msgId"].asString());
			Json::Value payloads = jsonData["payload"];
			switch (msgId) {
			case 101: // 101 : REGISTER_CONTACT 
				accountManager->handleRegisterContact(payloads);
				break;
			case 102: // 102 : LOGIN
				accountManager->handleLogin(payloads);
				break;
			case 103: // 103 : LOGOUT
				accountManager->handleLogout(payloads);
				break;
			case 104: // 104 : UPDATE_MY_CONTACTLIST
				accountManager->handleUpdateMyContactList(payloads);
				break;
			case 105: // 105 : RESET_PASSWORD
				accountManager->handleResetPassword(payloads);
				break;
			case 106: // 106 : GET_ALL_CONTACT
				accountManager->handleGetAllContact();
				break;
			default:
				break;
			}
		}
	}
	clientMap.erase(contactId);
	closesocket(clientSocket);
}

std::string SessionManager::GetClientName(int clientSocket)
{
	struct sockaddr_in clientAddress;
	socklen_t clientAddressLength = sizeof(clientAddress);

	getpeername(clientSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);

	char clientIP[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIP, INET_ADDRSTRLEN);
	int clientPort = ntohs(clientAddress.sin_port);

	std::string ip(clientIP);
	std::string portStr = std::to_string(clientPort);
	std::string displayName = "(" + ip + ":" + portStr + ")";

	return displayName;
}

void SessionManager::sendData(const char* data, std::string to) {
	send(clientMap[to], data, strlen(data), 0);
}

std::vector<std::string> SessionManager::split(const std::string& str, char delimiter) {
	std::vector<std::string> tokens;
	std::istringstream ss(str);
	std::string token;
	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens;
}