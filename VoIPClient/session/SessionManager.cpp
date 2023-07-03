#include <iostream>
#include <thread>
#include <sstream>
#include <winsock2.h>
#include <WS2tcpip.h>

#include "SessionManager.h"

SessionManager* SessionManager::instance = nullptr;
SOCKET clientSocket;

SessionManager::SessionManager() {
	strcpy_s(serverIP, sizeof(serverIP), "127.0.0.1");
	serverPort = 5555;

	callsListener = nullptr;
	accountListener = nullptr;
}

SessionManager* SessionManager::getInstance() {
	if (instance == nullptr) {
		instance = new SessionManager();
	}
	return instance;
}

void SessionManager::setCallsListener(CallsManagerListener* listener) {
	callsListener = listener;
}

void SessionManager::setAccountListener(AccountManagerListener* listener) {
	accountListener = listener;
}

void SessionManager::init(const char* ip, int port) {
	if (strlen(ip) > 0) {
		strcpy_s(serverIP, sizeof(serverIP), ip); // dynamic ip
	}
	if (port > 0) {
		serverPort = port; // dynamic port
	}
	std::thread sessionThread(&SessionManager::openSocket, instance);
	sessionThread.join();
}

// socket Receive function
void SessionManager::proc_recv() {
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
		if (msg.find("onLoginSuccess") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			accountListener->onLoginSuccess(tokens.back());
			continue;
		}
		if (msg.find("onIncomingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			std::string from = tokens[tokens.size()-2];
			std::string connId = tokens.back();
			callsListener->onIncomingCall(connId, from);
			continue;
		}
		if (msg.find("onSuccessfulOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			callsListener->onSuccessfulOutgoingCall(tokens.back());
			continue;
		}
		if (msg.find("onSuccessfulIncomingCall") != std::string::npos) {
			callsListener->onSuccessfulIncomingCall();
			continue;
		}
		if (msg.find("onFailedOutgoingCall") != std::string::npos) {
			std::vector<std::string> tokens = split(msg, ',');
			callsListener->onFailedOutgoingCall(tokens.back());
			continue;
		}
		if (msg.find("onRejectedIncomingCall") != std::string::npos) {
			callsListener->onRejectedIncomingCall();
			continue;
		}
		if (msg.find("onDisconnected") != std::string::npos) {
			callsListener->onDisconnected();
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
