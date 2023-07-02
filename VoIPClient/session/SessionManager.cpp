#include "SessionManager.h"

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <WS2tcpip.h>

#define PACKET_SIZE 1024

SessionManager* SessionManager::instance = nullptr;
std::thread sessionThread;
std::thread recvThread;

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
	sessionThread = std::thread(&SessionManager::openSocket, instance);
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
		if (msg.find("startOutgoingCall") != std::string::npos) {
			callsListener->startIncomingCall();
			continue;
		}
		if (msg.find("disconnectCall") != std::string::npos) {
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

	/*while (!WSAGetLastError()) {
		std::cout << "Enter message: ";
		std::string message;
		getline(std::cin, message);

		if (message.empty()) {
			std::cout << "Terminate connection" << std::endl;
			shutdown(clientSocket, SD_SEND);
			break;
		}

		if (sendData(message.c_str()) == -1) {
			std::cerr << "Fail to send message" << std::endl;
			break;
		}
	}*/
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
