#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "session/CallsManager.h"
#include "session/AccountManager.h"
#include "session/SessionManager.h"

int main() {
	// TEST CODE
	std::string IP, PORT;
	std::cout << "Input serverIP(127.0.0.1): ";
	getline(std::cin, IP);
	std::cout << "input serverPort(5555): ";
	getline(std::cin, PORT);
	int port = PORT.length() > 0 ? std::stoi(PORT) : 0;

	// UI Thread
	SessionManager* sessionManager = SessionManager::getInstance();
	AccountManager* accountManager = AccountManager::getInstance();
	CallsManager* callsManager = CallsManager::getInstance();
	sessionManager->setAccountListener(accountManager);
	sessionManager->setCallsListener(callsManager);

	accountManager->setSessionControl(sessionManager);
	callsManager->setSessionControl(sessionManager);

	//sessionManager->init(IP.c_str(), port);
	std::thread t(&SessionManager::init, sessionManager, IP.c_str(), port);

	// TEST CODE
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); //TEST
	accountManager->login();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		//std::cout << "Enter message: ";
		std::string message;
		getline(std::cin, message);

		if (message == "1") {
			std::cout << "Insert Contact Number: ";
			int num;
			std::cin >> num;
			std::cin.ignore();
			callsManager->startOutgoingCall(("CONTACT_" + std::to_string(num)).c_str());
			continue;
		}
		if (message == "5") {
			callsManager->answerCall();
			continue;
		}
		if (message == "6") {
			callsManager->rejectCall();
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