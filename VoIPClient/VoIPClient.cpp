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
	//sessionManager->init(IP.c_str(), port);
	std::thread t(&SessionManager::init, sessionManager, IP.c_str(), port);

	// TEST CODE
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); //TEST
	AccountManager::getInstance()->login();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		//std::cout << "Enter message: ";
		std::string message;
		getline(std::cin, message);
		
		if (message.empty() || message == "0") {
			// Exit
			SessionManager::releaseInstance();
			break;
		}
		else if (message == "1") {
			std::cout << "Insert Contact Number: ";
			int num;
			std::cin >> num;
			std::cin.ignore();
			CallsManager::getInstance()->startOutgoingCall(("CONTACT_" + std::to_string(num)).c_str());
		}
		else if (message == "5") {
			CallsManager::getInstance()->answerCall();
		}
		else if (message == "6") {
			CallsManager::getInstance()->rejectCall();
		}
		else if (message == "7") {
			CallsManager::getInstance()->disconnectCall();
		}
		else {
			sessionManager->sendData(message.c_str());
		}

		jsonCommandParser(message);
	}
	t.join();

	CallsManager::releaseInstance();
	AccountManager::releaseInstance();
	std::cout << "Exit Main Thread" << std::endl;

	return 0;
}