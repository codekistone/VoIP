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

void AccountManager::handleRegisterContact(Json::Value data, string from)
{
	std::cout << "handleRegisterContact() " << data << std::endl;
	Json::Value payload;
	// 1. Check if already registered
	string cid = data["cid"].asString();
	if (cid.empty()) {
		// No CID from client, use email as CID
		data["cid"] = data["email"];
		cid = data["cid"].asString();
	} 
	if (contactDb->get(cid) == NULL) {
		// No user exists : Add registration data to database
		contactDb->update(cid, data);
		payload["result"] = 0; // Success
		payload["reason"] = "Success";
	} else {
		// User already exists return error 
		payload["result"] = 1; // Failed
		payload["reason"] = "Contact Identifier Already exists";		
	}	
	sessionControl->sendData(makeMessage(101, payload).c_str(), from);
}

string AccountManager::handleLogin(Json::Value data, string from)
{	
	Json::Value payload;
	string cid = contactDb->search("email", data["email"].asString());
	if (!cid.empty()) {
		// Check password
		string storedPassword = contactDb->get(cid, "password").asString();
		if (data["password"] == storedPassword) {
			contactDb->update(cid, "login", true); // Update login status	
			payload["result"] = 0; // Success
			payload["myContactData"] = contactDb->get(cid);
			payload["myContactData"].removeMember("ipAddress");
			payload["myContactData"].removeMember("login");
			payload["myContactData"].removeMember("password");
			payload["myContactData"].removeMember("passwordAnswer");
			payload["myContactData"].removeMember("passwordQuestion");
			cout << "handleLogin()/OK:" << payload << endl;
		} else {
			// Wrong password
			payload["result"] = 2; // Fail (Wrong password) 
			cout << "handleLogin()/FAIL: Wrong password" << endl;

		}
	} else {
		payload["result"] = 1; // Fail (Not registered)
		cout << "handleLogin()/FAIL:No contact data to login" << endl;
	}
	sessionControl->sendData(makeMessage(102, payload).c_str(), from);
	return cid;
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

