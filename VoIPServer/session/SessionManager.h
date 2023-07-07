#pragma once

#include <map>
#include <string>
#include <vector>
#include <thread>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "SessionControl.h"
#include "ITelephonyManager.h"
#include "IAccountManager.h"

#define USE_TLS 1

constexpr auto PACKET_SIZE = 1024;

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;
#if USE_TLS
	int serverPort = 443;
#else
	int serverPort = 5555;
#endif
	
	const int MAX_CLIENTS = 10;  // 최대참여자 수
	std::map<std::string, int> clientMap;
	std::vector<std::thread> clientThread;

	int contactNum; // For TEST

	//For TLS
#if USE_TLS
	SSL_CTX* ctx;
	std::map<std::string, SSL*> clientSSLMap;
#endif

	ITelephonyManager* telephonyManager;
	IAccountManager* accountManager;

	SessionManager();

public:
	static SessionManager* getInstance();
	static void releaseInstance();

	void init();
	void release();
	void openSocket();
	void HandleClient(SSL* ssl, int clientSocket);
	std::string GetClientName(int clientSocket);

	// interface
	void sendData(const char* message, std::string to) override;
	void sendData(int msgId, Json::Value payload, std::string to) override;
};

