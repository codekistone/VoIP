#pragma once

#include "session/SessionManager.h"
#include "session/AccountManager.h"
#include "session/CallsManager.h"

class CommandLineInterface {
private:
	static CommandLineInterface* instance;
public:
	static CommandLineInterface* getInstance();
	void getServerInfo(std::string &ip, int& port);
	void startCli(AccountManager* am, CallsManager* cm);
};