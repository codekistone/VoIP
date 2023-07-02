#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "session/CallsManager.h"
#include "session/AccountManager.h"
#include "session/SessionManager.h"

int main() {
	std::string IP, PORT;
	std::cout << "Input serverIP(127.0.0.1): ";
	getline(std::cin, IP);
	std::cout << "input serverPort(5555): ";
	getline(std::cin, PORT);
	int port = PORT.length() > 0 ? std::stoi(PORT) : 0;

	SessionManager* sessionManager = SessionManager::getInstance();
	CallsManager* callsManager = CallsManager::getInstance();
	AccountManager* accountManager = AccountManager::getInstance();
	sessionManager->setCallsListener(callsManager);
	sessionManager->setAccountListener(accountManager);

	callsManager->setSessionControl(sessionManager);
	accountManager->setSessionControl(sessionManager);

	//sessionManager->init(IP.c_str(), port);

	// TEST CODE
	std::thread t(&SessionManager::init, sessionManager, IP.c_str(), port);

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		std::cout << "Enter message: ";
		std::string message;
		getline(std::cin, message);

		if (message == "1") {
			callsManager->startOutgoingCall("CONTACT_01");
			continue;
		}

		if (message == "7") {
			callsManager->disconnectCall();
			continue;
		}

		sessionManager->sendData(message.c_str());
		if (message.empty()) {
			break;
		}
	}
	t.join();

	return 0;
}