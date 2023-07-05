#include <iostream>

#include "AccountManager.h"

AccountManager* AccountManager::instance = nullptr;


AccountManager::AccountManager() {
	sessionControl = nullptr;
	contactDb = ContactDb::getInstance();
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
void AccountManager::handleLogin_(std::string requester) {
	sessionControl->sendData(("onLoginSuccess," + requester).c_str(), requester);
}

void AccountManager::handleRegisterContact(Json::Value data)
{
	cout << "handleRegisterContact() " << data << endl;
}

void AccountManager::handleLogin(Json::Value data)
{
	cout << "handleLogin() " << data << endl;
}

void AccountManager::handleLogout(Json::Value data)
{
	cout << "handleLogout() " << data << endl;
}

void AccountManager::handleUpdateMyContactList(Json::Value data)
{
	cout << "handleUpdateMyContactList() " << data << endl;
}

void AccountManager::handleResetPassword(Json::Value data)
{
	cout << "handleResetPassword() " << data << endl;
}

void AccountManager::handleGetAllContact()
{
	cout << "handleGetAllContact() " << endl;
}

