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
	serverPort = 443;

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
	Json::Reader jsonReader;
	while (true) {
		memset(buf, 0, sizeof(buf));
#if USE_TLS
		SSIZE_T bytesRead = BIO_read(bio, buf, PACKET_SIZE);
#else
		SSIZE_T bytesRead = recv(clientSocket, buf, PACKET_SIZE, 0);
#endif
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
		if (msg.find("onLoginSuccess") != std::string::npos) {
			msgStr = "onLoginSuccess";
			std::vector<std::string> tokens = split(msg, ',');
			accountManager->onLoginSuccess(tokens.back());
		}

		//-------------------------------------------------------------
		// JSON payload parser
		Json::Value jsonData;
		if (jsonReader.parse(msg, jsonData) == true) {
			// received data parsed as JSON data			
			int msgId = std::stoi(jsonData["msgId"].asString());
			Json::Value payloads = jsonData["payload"];
			std::cout << "--------------------------------------------------------" << std::endl;
			std::cout << "RECEIVE / " << msgId << " : " << payloads << std::endl;
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
				callsManager->onJoinConferenceResult(payloads);
				break;
			case 209: // 209 : EXIT_CONFERENCE
				msgStr = "EXIT_CONFERENCE";
				callsManager->onExitConference(payloads);
				break;
			case 301: // 301 : OUTGOING_CALL_RESULT
				msgStr = "OUTGOING_CALL_RESULT";
				callsManager->onOutgoingCallResult(payloads);
				break;
			case 302: // 302 : INCOMING_CALL
				msgStr = "INCOMING_CALL";
				callsManager->onIncomingCall(payloads);
				break;
			case 303: // 303 : INCOMING_CALL_RESULT
				msgStr = "INCOMING_CALL_RESULT";
				callsManager->onIncomingCallResult(payloads);
				break;
			case 305: // 305 : DISCONNECT_CALL
				msgStr = "DISCONNECT_CALL";
				callsManager->onDisconnected(payloads);
				break;
			default:
				break;
			}
		}

		// std::cout << "Received message from server: [" << msgStr << "] " << buf << std::endl;
	}
}

int verify_callback(int preverify_ok, X509_STORE_CTX* ctx)
{
	return 1; //for demo
	if (preverify_ok == 0)
	{
		int depth = X509_STORE_CTX_get_error_depth(ctx);
		int err = X509_STORE_CTX_get_error(ctx);
		printf("Certificate verification error: depth=%d, err=%d\n", depth, err);
		return 0;
	}

	return 1;
}

// socket Open function
void SessionManager::openSocket() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		std::cout << "WSA error";
		WSACleanup();
		return;
	}

#if USE_TLS

	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	const SSL_METHOD* method = TLS_client_method();
	if (method == NULL)
	{
		printf("Failed to create TLS client method.\n");
		WSACleanup();
		return;
	}

	ctx = SSL_CTX_new(method);
	if (ctx == NULL)
	{
		printf("Failed to create SSL context.\n");
		WSACleanup();
		return;
	}

	if (SSL_CTX_set_cipher_list(ctx, "AES128-SHA256") != 1) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
	}

	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);
	bio = BIO_new_ssl_connect(ctx);
	if (bio == NULL)
	{
		printf("Failed to create BIO.\n");
		SSL_CTX_free(ctx);
		WSACleanup();
		return;
	}
	std::string hostname = serverIP;
	hostname += ":" + std::to_string(serverPort);
	BIO_set_conn_hostname(bio, hostname.c_str());
	//BIO_set_nbio(bio, 1);
	if (BIO_do_connect(bio) <= 0)
	{
		if (!BIO_should_retry(bio))
		{
			printf("Failed to connect.\n");
			BIO_free_all(bio);
			SSL_CTX_free(ctx);
			WSACleanup();
			return;
		}
	}

	if (BIO_do_handshake(bio) <= 0) {
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
		return;
	}
	printf("Connected to the server.\n");

	std::thread recvThread(&SessionManager::proc_recv, instance);
	recvThread.join();
	BIO_free_all(bio);
	SSL_CTX_free(ctx);
#else

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
#endif
	WSACleanup();
}

int SessionManager::sendData(const char* data) {
	if (strlen(data) == 0) {
		std::cout << "Terminate connection" << std::endl;
		shutdown(clientSocket, SD_SEND);
		return 0;
	}
#if USE_TLS
	return BIO_write(bio, data, strlen(data));
#else
	return send(clientSocket, data, strlen(data), 0);
#endif
}

int SessionManager::sendData(int msgId, Json::Value payload) {
	Json::FastWriter fastWriter;
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;
	std::string jsonString = fastWriter.write(root);
	const char* data = jsonString.c_str();

#if USE_TLS
	return BIO_write(bio, data, strlen(data));
#else
	return send(clientSocket, data, strlen(data), 0);
#endif
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
