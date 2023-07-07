#include <iostream>
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

	std::string contactId = GetClientName(clientSocket);
	clientMap.insert({ contactId, clientSocket });
	char buffer[PACKET_SIZE];
	Json::Reader jsonReader;
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
			telephonyManager->releaseConnection(contactId);
			break;
		}

		// listener test
		std::string msg(buffer);
		std::string msgStr;
		if (msg.find("Login") != std::string::npos) {
			msgStr = "LOGIN";
			contactId = ("CONTACT_" + std::to_string(contactNum++));
			clientMap.insert({ contactId, clientSocket });
			accountManager->handleLogin_(contactId.c_str());
		}

		//-------------------------------------------------------------
		// JSON payload parser
		Json::Value jsonData;
		if (jsonReader.parse(msg, jsonData) == true) {
			// received data parsed as JSON data			
			string ipAddress = GetClientName(clientSocket);
			int msgId = std::stoi(jsonData["msgId"].asString());
			Json::Value payloads = jsonData["payload"];
			std::cout << "--------------------------------------------------------" << std::endl;
			std::cout << "RECEIVE[" << msgId << "]IP[" << ipAddress << "]CID[" << contactId  << "]PAYLOAD:" << payloads << std::endl;
			std::cout << "--------------------------------------------------------" << std::endl;
			switch (msgId) {
			case 101: // 101 : REGISTER_CONTACT 		
				msgStr = "REGISTER_CONTACT";
				accountManager->handleRegisterContact(payloads, contactId);
				break;
			case 102: // 102 : LOGIN		
				{
					msgStr = "LOGIN";					
					string cid = accountManager->handleLogin(payloads, ipAddress, contactId);
					if (!cid.empty()) {		
					    // Replace client map(IP Address:Port) to valid contactId after logged in				
						clientMap.erase(contactId);
						contactId = cid;
						clientMap.insert({ cid, clientSocket });
					}
				}
				break;
			case 103: // 103 : LOGOUT
				msgStr = "LOGOUT";
				if (accountManager->handleLogout(payloads)) {
					// Erase contactID -> socket map when logged out
					clientMap.erase(contactId);
					contactId = GetClientName(clientSocket);
					clientMap.insert({ contactId, clientSocket });
				}
				break;
			case 104: // 104 : UPDATE_MY_CONTACTLIST
				msgStr = "UPDATE_MY_CONTACTLIST";
				accountManager->handleUpdateMyContactList(payloads, contactId);
				break;
			case 105: // 105 : RESET_PASSWORD
				msgStr = "RESET_PASSWORD";
				accountManager->handleResetPassword(payloads, contactId);
				break;
			case 106: // 106 : GET_ALL_CONTACT
				msgStr = "GET_ALL_CONTACT";
				accountManager->handleGetAllContact(contactId);
				break;
			case 206: // 206 : CREATE_CONFERENCE
				msgStr = "CREATE_CONFERENCE";
				telephonyManager->handleCreateConference(payloads);
				accountManager->handleCreateConference(payloads, contactId);
				break;
			case 208: // 208 : JOIN_CONFERENCE
				msgStr = "JOIN_CONFERENCE";
				payloads["from"] = contactId;
				telephonyManager->handleJoinConference(payloads);
				break;
			case 209: // 209 : EXIT_CONFERENCE
				msgStr = "EXIT_CONFERENCE";
				payloads["from"] = contactId;
				telephonyManager->handleExitConference(payloads);
				break;
			case 301: // 301 : OUTGOING_CALL
				{
					msgStr = "OUTGOING_CALL";
					payloads["from"] = contactId;
					std::string to(payloads["to"].asString());
					if (clientMap.count(to) == 0) {
						telephonyManager->handleOutgoingCallNoUser(payloads);
						break;
					}
					telephonyManager->handleOutgoingCall(payloads);
					break;
				}
			case 302: // 302 : INCOMING_CALL_RESPONSE
				msgStr = "INCOMING_CALL_RESPONSE";
				payloads["from"] = contactId;
				telephonyManager->handleIncomingCallResponse(payloads);
				break;
			case 305: // 305 : DISCONNECT_CALL
				msgStr = "DISCONNECT_CALL";
				telephonyManager->handleDisconnect(payloads);
				break;
			default:
				break;
			}
		}
		//std::cout << "Received message from client [" << displayName << "][" << msgStr << "]" << std::endl << buffer << std::endl;
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

	//return displayName;
	return ip;
}

void SessionManager::sendData(const char* data, std::string to) {
	send(clientMap[to], data, strlen(data), 0);
}

void SessionManager::sendData(int msgId, Json::Value payload, std::string to) {
	Json::FastWriter fastWriter;
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;
	std::string jsonString = fastWriter.write(root);
	const char* data = jsonString.c_str();
	send(clientMap[to], data, strlen(data), 0);
}