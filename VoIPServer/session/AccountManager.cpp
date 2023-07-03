#include <iostream>

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

// Implement listener
void AccountManager::handleLogin(std::string requester) {
	sessionControl->sendData(("onLoginSuccess," + requester).c_str(), requester);
}