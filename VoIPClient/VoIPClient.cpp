#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "session/CallsManager.h"
#include "session/AccountManager.h"
#include "session/SessionManager.h"
#include "CommandLineInterface.h"

int main() {	
	SessionManager* sessionManager = SessionManager::getInstance();
	AccountManager* accountManager = AccountManager::getInstance();
	CallsManager* callsManager = CallsManager::getInstance();

	// Get server information
	std::string serverIp; int serverPort;	
	CommandLineInterface* cli = CommandLineInterface::getInstance();
	cli->getServerInfo(&serverIp, &serverPort);

	// Start session thread
	std::thread t(&SessionManager::init, sessionManager, serverIp.c_str(), serverPort);	

	// Start CLI
	cli->startCli(accountManager, callsManager);	
	
	// Join session thread
	t.join();

	sessionManager->releaseInstance();
	callsManager->releaseInstance();
	accountManager->releaseInstance();

	return 0;
}