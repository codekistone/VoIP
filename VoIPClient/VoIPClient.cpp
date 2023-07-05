#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "session/CallsManager.h"
#include "session/AccountManager.h"
#include "session/SessionManager.h"
#pragma comment (lib, "../json/jsoncpp_static.lib")

void jsonCommandParser( std::string message ) {
	try {
		if (message != "j") {
			return;
		}
		SessionManager* sessionManager = SessionManager::getInstance();
		std::cout << "** ENTER JSON TEXT Data : " << std::endl;
		std::string line;
		std::string lines;
		while (getline(std::cin, line)) {
			if (line.empty()) {
				break;
			}
			lines += line + "\n";
		}
		Json::Value jsonData;
		Json::Reader reader;
		bool result = reader.parse(lines, jsonData);
		if (result == false) {
			std::cout << "Json : PARSE FAILED" << std::endl;
			return;
		}
		Json::FastWriter fastWriter;
		std::string jsonDataString = fastWriter.write(jsonData);
		int retValue = sessionManager->sendData(jsonDataString.c_str());
		if (retValue > 0) {
			std::cout << "SENT (msgId : " << jsonData["msgId"] << ")" << std::endl;
		}		
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}
}

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

		jsonCommandParser(message);

		if (message.empty()) {
			break;
		}
	}
	t.join();

	return 0;
}