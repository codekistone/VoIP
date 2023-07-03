#include <iostream>
#include <sstream>

#include "SessionManager.h"

SessionManager* SessionManager::instance = nullptr;

SessionManager::SessionManager() {
	contactNum = 0;

	telephonyListener = nullptr;
	accountListener = nullptr;
}

SessionManager* SessionManager::getInstance() {
	if (instance == nullptr) {
		instance = new SessionManager();
	}
	return instance;
}

void SessionManager::init() {
	std::thread sessionThread(&SessionManager::openSocket, instance);
	sessionThread.join();
}

void SessionManager::setTelephonyListener(TelephonyManagerListener* listener) {
	telephonyListener = listener;
}

void SessionManager::setAccountListener(AccountManagerListener* listener) {
	accountListener = listener;
}

void SessionManager::openSocket() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize Winsock." << std::endl;
		return;
	}

	SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	while (true) {
		SOCKADDR_IN clientAddress{};
		int clientAddressLengh = sizeof(clientAddress);
		SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressLengh);
		if (clientSocket == -1) {
			std::cerr << "Failed to accept client connection." << std::endl;
			continue;
		}

		// 클라이언트와의 통신을 별도 스레드로 처리
		std::thread clientThread(&SessionManager::HandleClient, instance, clientSocket);
		clientThread.detach();
	}

	// 서버 소켓 닫기
	closesocket(serverSocket);
	WSACleanup();
}

void SessionManager::HandleClient(int clientSocket) {
	// 클라이언트와의 통신 처리
	// 예: 메시지 송수신, 데이터 처리 등

	std::string displayName = GetClientName(clientSocket);

	std::cout << "Connected to client! " << displayName << std::endl;

	std::string contactId;
	char buffer[PACKET_SIZE];
	while (true) {
		memset(buffer, 0, sizeof(buffer));

		// 클라이언트로부터 메세지 수신
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
			accountListener->handleLogin(contactId.c_str());
		}
		else if (msg.find("startOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string to(tokens.back());
			if (clientMap.count(to) == 0) {
				sendData("onFailedOutgoingCall,unreachable", contactId);
				continue;
			}
			telephonyListener->handleOutgoingCall(contactId, to);
		}
		else if (msg.find("answerCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			telephonyListener->handleAnswer(tokens.back(), contactId);
		}
		else if (msg.find("rejectCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string cause = tokens[tokens.size() - 2];
			telephonyListener->handleReject(tokens.back(), cause, contactId);
		}
		else if (msg.find("disconnectCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			telephonyListener->handleDisconnect(tokens.back());
		}

		std::cout << "Received message from client" << displayName << ": " << buffer << std::endl;
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