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

void AccountManager::releaseInstance() {
	if (instance != nullptr) {
		instance->setSessionControl(nullptr);
		delete instance;
		instance = nullptr;
		std::cout << "AccountManager::releaseInstance" << std::endl;
	}
}

// Implement interface
void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void AccountManager::handleLogin_(std::string requester) {
	sessionControl->sendData(("onLoginSuccess," + requester).c_str(), requester);
}

void AccountManager::handleRegisterContact(Json::Value data, string from)
{	
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
		if (!data["password"].empty() &&
			!data["password_question"].empty() &&
			!data["password_answer"].empty()) {		
			payload["result"] = 0; // Success
			payload["reason"] = "Success";
			cout << "handleRegisterContact()/OK:" << payload << endl;
		}
		else {
			// Mandatory items are missing
			payload["result"] = 2; // Failed
			payload["reason"] = "Mandatory items are missing";
			cout << "handleRegisterContact()/FAIL: Mandatory items are missing" << endl;
		}
	} else {
		// User already exists return error 
		payload["result"] = 1; // Failed
		payload["reason"] = "Contact Identifier Already exists";		
		cout << "handleRegisterContact()/FAIL: Contact Identifier Already exists" << endl;
	}	
	/*
		result 0 : SUCCESS
		result 1 : FAILED (ALREADY REGISTERED)
		result 2 : FAILED (MANDATORY ITEMS ARE MISSING)
	*/
	sessionControl->sendData(101, payload, from);
}

string AccountManager::handleLogin(Json::Value data, string ipAddress, string from)
{	
	Json::Value payload;
	// Get Unique contact id from Database using received email
	string cid = contactDb->search("email", data["email"].asString());
	if (!cid.empty()) { 
		// Contact id exists in database
		// Check password
		string storedPassword = contactDb->get(cid, "password").asString();
		if (data["password"] == storedPassword) {
			// Password patch 
			contactDb->update(cid, "login", true); // Update login status	
			contactDb->update(cid, "ipAddress", ipAddress); // Update IP address
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
			payload["result"] = 2; // Fail 
			cout << "handleLogin()/FAIL: Wrong password" << endl;

		}
	} else {
		// Not registered
		payload["result"] = 1; // Fail 
		cout << "handleLogin()/FAIL:No contact data to login" << endl;
	}
	/*
		result 0 : SUCCESS
		result 1 : FAILED (NOT REGISTERED)
		result 2 : FAILED (WRONG PASSWORD)
	*/
	sessionControl->sendData(102, payload, from);
	return cid;
}

bool AccountManager::handleLogout(Json::Value data)
{
	bool retValue = false;
	string cid = data["cid"].asString();	
	if (!cid.empty()) {
		if (contactDb->get(cid, "login") == true) { // Already loggined in
			contactDb->update(cid, "login", false); // Update login status
			contactDb->remove(cid, "ipAddress");// Update IP address
			retValue = true;
			std::cout << "handleLogout()/OK:" << data << std::endl;
		}
		else {
			std::cout << "handleLogout()/FAIL:Not logged in:" << data << std::endl;
		}
	}
	else {
		std::cout << "handleLogout()/FAIL:No CID:" << data << std::endl;
	}
	
	return retValue;
}

void AccountManager::handleUpdateMyContactList(Json::Value data, string from)
{
	// No response message for 104
	string cid = data["cid"].asString();
	Json::Value updateContactList = data["myContactList"];
	if (!cid.empty() && !updateContactList.empty()) {
		Json::Value myContactList = contactDb->get(cid, "myContactList");
		if (contactDb->update(cid, "myContactList", updateContactList)) {			
			std::cout << "handleUpdateMyContactList()/OK:" << contactDb->get(cid, "myContactList") << std::endl;
		}
		else {
			std::cout << "handleUpdateMyContactList()/FAILED:" << updateContactList << std::endl;
		}
	}
	else {
		std::cout << "handleUpdateMyContactList()/FAIL:Payload not valid" << std::endl;
	}	
}

void AccountManager::handleResetPassword(Json::Value data, string from)
{
	Json::Value payload;
	string cid = data["cid"].asString();
	if (!cid.empty()) {
		string newPassword = data["password"].asString();
		int question = contactDb->get(cid, "passwordQuestion").asInt();
		string answer = contactDb->get(cid, "passwordAnswer").asString();		
		if (question >= 0 && 
			!answer.empty() &&
			data["passwordQuestion"] == question && 
			data["passwordAnswer"] == answer ) {
			contactDb->update(cid, "password", newPassword);		
			payload["result"] = 0;
			cout << "handleResetPassword()/OK:" << data << endl;
		}
		else {
			payload["result"] = 2;
			cout << "handleResetPassword()/FAIL:Incorrect Data:" << data << endl;
		}
	}
	else {
		payload["result"] = 1;
		cout << "handleResetPassword()/FAIL:No CID found:" << data << endl;
	}
	/*
		result 0 : SUCCESS
		result 1 : FAILED (NOT REGISTERED)
		result 2 : FAILED (UNKNOWN)
	*/
	sessionControl->sendData(105, payload, from);
}

void AccountManager::handleGetAllContact( string from)
{	
	Json::Value payload;
	Json::Value allContacts = contactDb->get();
	if (!allContacts.empty()) {
		payload = allContacts;
		for (int i = 0; i < allContacts.size(); i++) {
			payload[i]["cid"] = allContacts[i]["cid"];
			payload[i]["email"] = allContacts[i]["email"];
			payload[i]["name"] = allContacts[i]["name"];
			payload[i]["ipAddress"] = allContacts[i]["ipAddress"];
			payload[i]["myContactList"] = allContacts[i]["myContactList"];
		}
		std::cout << "handleGetAllContact()/OK:" << payload << std::endl;
	} else {
		std::cout << "handleGetAllContact()/FAILED:" << payload << std::endl;
	}
	sessionControl->sendData(106, payload, from);
}

