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
	myIpAddr = "";
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

	std::thread sessionThread(&SessionManager::openSocket, this);
	sessionThread.detach();
}

void SessionManager::release() {
	std::cout << "SessionManager release" << std::endl;
	// TODO thread 정리
	shutdown(clientSocket, SD_SEND);

	callsManager->setSessionControl(nullptr);
	accountManager->setSessionControl(nullptr);
}

void SessionManager::getMyIp()
{
	if (myIpAddr.length() <= 0) {
		struct addrinfo* _addrinfo;
		struct addrinfo* _res;
		char _address[INET6_ADDRSTRLEN];
		char szHostName[255];
		gethostname(szHostName, sizeof(szHostName));
		getaddrinfo(szHostName, NULL, 0, &_addrinfo);
		for (_res = _addrinfo; _res != NULL; _res = _res->ai_next) {
			if (_res->ai_family == AF_INET) {
				if (NULL != inet_ntop(AF_INET, &((struct sockaddr_in*)_res->ai_addr)->sin_addr, _address, sizeof(_address))) {
					myIpAddr = _address;
				}
			}
		}
	}
}
// socket Receive function
void SessionManager::proc_recv() {

	Json::Value root;
	Json::Reader reader;

	char buf[PACKET_SIZE];
	Json::Reader jsonReader;
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
		std::string msgStr;
		//-------------------------------------------------------------
		// JSON payload parser
		Json::Value jsonData;
		if (jsonReader.parse(msg, jsonData) == true) {
			// received data parsed as JSON data			
			int msgId = std::stoi(jsonData["msgId"].asString());
			Json::Value payloads = jsonData["payload"];
			std::cout << std::endl << "--------------------------------------------------------" << std::endl;
			std::cout << "RECEIVE[" << msgId << "]PAYLOAD:" << payloads << std::endl;
			std::cout << "--------------------------------------------------------" << std::endl;
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
			case 205: // 205 : GET_ALL_CONFERENCE
				msgStr = "GET_ALL_CONFERENCE";
				accountManager->handleGetAllMyConference(payloads);
				break;
			case 208: // 208 : JOIN_CONFERENCE
				msgStr = "JOIN_CONFERENCE";
				payloads["serverIp"] = serverIP;
				payloads["myIp"] = myIpAddr;
				callsManager->onJoinConferenceResult(payloads);
				break;
			case 209: // 209 : EXIT_CONFERENCE
				msgStr = "EXIT_CONFERENCE";
				callsManager->onExitConference(payloads);
				break;
			case 301: // 301 : OUTGOING_CALL_RESULT
				msgStr = "OUTGOING_CALL_RESULT";
				payloads["serverIp"] = serverIP;
				payloads["myIp"] = myIpAddr;
				callsManager->onOutgoingCallResult(payloads);
				break;
			case 302: // 302 : INCOMING_CALL
				msgStr = "INCOMING_CALL";
				payloads["serverIp"] = serverIP;
				callsManager->onIncomingCall(payloads);
				break;
			case 303: // 303 : INCOMING_CALL_RESULT
				msgStr = "INCOMING_CALL_RESULT";
				payloads["myIp"] = myIpAddr;
				callsManager->onIncomingCallResult(payloads);
				break;
			case 305: // 305 : DISCONNECT_CALL
				msgStr = "DISCONNECT_CALL";
				callsManager->onDisconnected(payloads);
				break;
			case 402: // 402 : 
				msgStr = "NOTIFY_VIDEO_QUALITY";
				callsManager->onVideoQualityChanged(payloads);
				break;
			default:
				break;
			}
		}

		// std::cout << "Received message from server: [" << msgStr << "] " << buf << std::endl;
	}
}

// socket Open function
void SessionManager::openSocket() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		accountManager->handleConnect(1); // Connection failed
		std::cout << "WSA error";
		WSACleanup();
		return;
	}
	getMyIp();
	clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		accountManager->handleConnect(1); // Connection failed
		std::cout << std::endl;
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
		accountManager->handleConnect(1); // Connection failed
		std::cout << std::endl;
		std::cerr << "Failed to connect server." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	std::cout << "Connected to server!" << std::endl;
	accountManager->handleConnect(0); // Connected to server
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

int SessionManager::sendData(int msgId, Json::Value payload) {
	Json::FastWriter fastWriter;
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;
	std::string jsonString = fastWriter.write(root);
	const char* data = jsonString.c_str();
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
