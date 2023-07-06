#pragma once

#include "session/SessionManager.h"
#include "session/AccountManager.h"
#include "session/CallsManager.h"

class CommandLineInterface {
private:
	static CommandLineInterface* instance;
	CommandLineInterface();

	SessionManager* sessionManager;
	AccountManager* accountManager;
	CallsManager* callsManager;

public:
	static CommandLineInterface* getInstance();
	void startCommandCli();
};