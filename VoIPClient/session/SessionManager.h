#pragma once

#include <string>
#include <vector>

#include "SessionControl.h"
#include "ICallsManager.h"
#include "IAccountManager.h"

constexpr auto PACKET_SIZE = 1024;

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	char serverIP[20];
	int serverPort;

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
};