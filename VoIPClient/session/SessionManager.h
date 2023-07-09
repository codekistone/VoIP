#pragma once

#include <string>
#include <vector>

#include "SessionControl.h"
#include "ICallsManager.h"
#include "IAccountManager.h"
#include "IUiController.h"

constexpr auto PACKET_SIZE = 10240;

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	char serverIP[20];
	int serverPort;

	std::string myIp;

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
	void getMyIp();

	// listener
	int sendData(const char* message) override;
	int sendData(int msgId, Json::Value payload) override;
};