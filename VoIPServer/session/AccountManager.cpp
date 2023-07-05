#include <iostream>

#include "AccountManager.h"

AccountManager* AccountManager::instance = nullptr;

// TODO : Should move utility method to somewhere else
std::string makeMessage(int msgId, Json::Value payload) {
	Json::FastWriter fastWriter;
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;
	std::string jsonString = fastWriter.write(root);
	return jsonString;
}

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

void AccountManager::releaseInstance() {
	if (instance != nullptr) {
		instance->setSessionControl(nullptr);
		delete instance;
		instance = nullptr;
		std::cout << "AccountManager::releaseInstance" << std::endl;
	}
}

// Implement listener
void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void AccountManager::handleLogin_(std::string requester) {
	sessionControl->sendData(("onLoginSuccess," + requester).c_str(), requester);
}

void AccountManager::handleRegisterContact(Json::Value data)
{
	std::cout << "handleRegisterContact() " << data << std::endl;
}

void AccountManager::handleLogin(Json::Value data)
{
	std::cout << "handleLogin() " << data << std::endl;
}

void AccountManager::handleLogout(Json::Value data)
{
	std::cout << "handleLogout() " << data << std::endl;
}

void AccountManager::handleUpdateMyContactList(Json::Value data)
{
	std::cout << "handleUpdateMyContactList() " << data << std::endl;
}

void AccountManager::handleResetPassword(Json::Value data)
{
	std::cout << "handleResetPassword() " << data << std::endl;
}

void AccountManager::handleGetAllContact()
{
	std::cout << "handleGetAllContact() " << std::endl;
}

