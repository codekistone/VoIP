#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "session/CallsManager.h"
#include "session/AccountManager.h"
#include "session/SessionManager.h"
#include "CommandLineInterface.h"

int main() {
	// TEST CODE	
	CommandLineInterface* cli = CommandLineInterface::getInstance();
	CommandLineInterface::getInstance()->startCommandCli();

	SessionManager::releaseInstance();
	CallsManager::releaseInstance();
	AccountManager::releaseInstance();

	std::cout << "Exit Main Thread" << std::endl;
	return 0;
}