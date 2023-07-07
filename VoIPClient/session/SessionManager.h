#pragma once

#include <string>
#include <vector>

#include "SessionControl.h"
#include "ICallsManager.h"
#include "IAccountManager.h"

constexpr auto PACKET_SIZE = 10240;

//For TLS
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#define USE_TLS 1

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	char serverIP[20];
	int serverPort;

	//For TLS
#if USE_TLS
	SSL_CTX* ctx;
#endif
	BIO* bio; //Need for function(proc_recv, sedData)

	ICallsManager* callsManager;
	IAccountManager* accountManager;
	
	SessionManager();

public:
	static SessionManager* getInstance();
	static void releaseInstance();

	void init(const char* ip, int port);
	void release();
	void openSocket();
	void proc_recv();
	std::vector<std::string> split(const std::string&, char);

	// listener
	int sendData(const char* message) override;
	int sendData(int msgId, Json::Value payload) override;
};