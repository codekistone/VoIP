#include <iostream>
#include <thread>
#include <chrono>

#include "AccountManager.h"

AccountManager* AccountManager::instance = nullptr;

AccountManager::AccountManager() {
	sessionControl = nullptr;
}

AccountManager* AccountManager::getInstance() {
	if (instance == nullptr) {
		instance = new AccountManager();
	}
	return instance;
}

void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void AccountManager::login() {
	// do login
	std::cout << "ACCOUNT :: doLogin" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("Login");
}

// Implement listener
void AccountManager::onLoginSuccess() {
	std::cout << "[Received] -> onLoginSuccess" << std::endl;
}