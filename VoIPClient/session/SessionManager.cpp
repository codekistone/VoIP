#include <iostream>
#include <thread>
#include <sstream>
#include <winsock2.h>
#include <WS2tcpip.h>

#include "SessionManager.h"
#include "CallsManager.h"
#include "AccountManager.h"

SessionManager* SessionManager::instance = nullptr;
SOCKET clientSocket;

SessionManager::SessionManager() {
	strcpy_s(serverIP, sizeof(serverIP), "127.0.0.1");
	serverPort = 5555;

	callsManager = CallsManager::getInstance();
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

void SessionManager::init(const char* ip, int port) {
	if (strlen(ip) > 0) {
		strcpy_s(serverIP, sizeof(serverIP), ip); // dynamic ip
	}
	if (port > 0) {
		serverPort = port; // dynamic port
	}

	callsManager->setSessionControl(this);
	accountManager->setSessionControl(this);

	std::thread sessionThread(&SessionManager::openSocket, instance);
	sessionThread.join();
}

void SessionManager::release() {
	std::cout << "SessionManager release" << std::endl;
	// TODO thread 정리
	shutdown(clientSocket, SD_SEND);

	callsManager->setSessionControl(nullptr);
	accountManager->setSessionControl(nullptr);
}

// socket Receive function
void SessionManager::proc_recv() {

	Json::Value root;
	Json::Reader reader;
	

	char buf[PACKET_SIZE];
	while (true) {
		memset(buf, 0, sizeof(buf));
		SSIZE_T bytesRead = recv(clientSocket, buf, PACKET_SIZE, 0);
		if (bytesRead == -1) {
			std::cerr << "Failed to receive response." << std::endl;
			//TODO FIX
			break;
		}
		if (bytesRead == 0) {
			std::cout << "Disconnected to server." << std::endl;
			break;
		}
		
		// listener test
		
		std::string msg(buf);
	
		//-------------------------------------------------------------
		// JSON payload parser
		Json::Reader jsonReader;
		Json::Value jsonData;
		if (jsonReader.parse(msg, jsonData) == true) {
			// received data parsed as JSON data			
			int msgId = std::stoi(jsonData["msgId"].asString());
			Json::Value payloads = jsonData["payload"];
			std::cout << msgId << ":" << payloads << std::endl;
			switch (msgId) {
			case 101: // 101 : REGISTER_CONTACT 
				accountManager->handleRegisterContact(payloads);
				break;
			case 102: // 102 : LOGIN
				accountManager->handleLogin(payloads);
				break;
			case 103: // 103 : LOGOUT
				break;
			case 104: // 104 : UPDATE_MY_CONTACTLIST
				break;
			case 105: // 105 : RESET_PASSWORD
				accountManager->handleResetPassword(payloads);
				break;
			case 106: // 106 : GET_ALL_CONTACT
				accountManager->handleGetAllContact(payloads);
				break;
			default:
				
				break;
			}
		}

		if (msg.find("onLoginSuccess") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			accountManager->onLoginSuccess(tokens.back());
			continue;
		}
		else if (msg.find("onIncomingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string from = tokens[tokens.size()-2];
			std::string connId = tokens.back();
			callsManager->onIncomingCall(connId, from);
			continue;
		}
		else if (msg.find("onSuccessfulOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			callsManager->onSuccessfulOutgoingCall(tokens.back());
			continue;
		}
		else if (msg.find("onSuccessfulIncomingCall") != std::string::npos) {
			callsManager->onSuccessfulIncomingCall();
			continue;
		}
		else if (msg.find("onFailedOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			callsManager->onFailedOutgoingCall(tokens.back());
			continue;
		}
		else if (msg.find("onRejectedIncomingCall") != std::string::npos) {
			callsManager->onRejectedIncomingCall();
			continue;
		}
		else if (msg.find("onDisconnected") != std::string::npos) {
			callsManager->onDisconnected();
			continue;
		}

		std::cout << "Received message from server: " << buf << std::endl;
	}
}

// socket Open function
void SessionManager::openSocket() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		std::cout << "WSA error";
		WSACleanup();
		return;
	}

	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cout << "socket error";
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	SOCKADDR_IN addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverPort); // 서버PORT
	inet_pton(AF_INET, serverIP, &(addr.sin_addr.s_addr));

	if (connect(clientSocket, (SOCKADDR*)&addr, sizeof(addr)) == -1) {
		std::cerr << "Failed to connect server." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	std::cout << "Connected to server!" << std::endl;

	std::thread recvThread(&SessionManager::proc_recv, instance);
	recvThread.join();

	closesocket(clientSocket);
	WSACleanup();
}

int SessionManager::sendData(const char* data) {
	if (strlen(data) == 0) {
		std::cout << "Terminate connection" << std::endl;
		shutdown(clientSocket, SD_SEND);
		return 0;
	}
	return send(clientSocket, data, strlen(data), 0);
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
