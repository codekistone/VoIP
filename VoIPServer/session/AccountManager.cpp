#include <iostream>

#include "AccountManager.h"

AccountManager* AccountManager::instance = nullptr;

AccountManager::AccountManager() {
	sessionControl = nullptr;
	contactDb = ContactDb::getInstance();
	conferenceDb = ConferenceDb::getInstance();
}

AccountManager* AccountManager::getInstance() {
	if (instance == nullptr) {
		instance = new AccountManager();
	}
	return instance;
}

void AccountManager::releaseInstance() {
	if (instance != nullptr) {
		instance->release();
		delete instance;
		instance = nullptr;
		std::cout << "AccountManager::releaseInstance" << std::endl;
	}
}

void AccountManager::release() {
	sessionControl = nullptr;
	contactDb = nullptr;
	conferenceDb = nullptr;
}

// Implement interface
void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
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
	bool cidExists = (contactDb->get(cid) == NULL) ? false : true;
	bool emailExists = false;
	string searchCid = contactDb->search("email", data["email"].asString());
	if (!searchCid.empty()) {
		emailExists = true;
	}

	if (!emailExists && !cidExists) {
		// No user exists : Add registration data to database
		contactDb->update(cid, data);
		if (!data["password"].empty() &&
			!data["passwordQuestion"].empty() &&
			!data["passwordAnswer"].empty()) {		
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
		// User already exists return error )
		payload["result"] = 1; // Failed
		if (emailExists) {
			payload["reason"] = "Same Email Already used by " + searchCid;
			cout << "handleRegisterContact()/FAIL: Same Email Already used by" << searchCid << endl;
		}
		else {
			payload["reason"] = "Contact Identifier Already exists";
			cout << "handleRegisterContact()/FAIL: Contact Identifier Already exists" << endl;

		}

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
	bool result = false;
	Json::Value payload;
	// Get Unique contact id from Database using received email
	string cid = contactDb->search("email", data["email"].asString());
	if (!cid.empty()) { 
		// Contact id exists in database
		// Check password
		string storedPassword = contactDb->get(cid, "password").asString();
		if (data["password"] == storedPassword) {
			if (contactDb->get(cid, "login") == true) {
				// TODO : Do we have to check already logged in state?
			}
			// Password patch 
			contactDb->update(cid, "login", true); // Update login status	
			contactDb->update(cid, "ipAddress", ipAddress); // Update IP address
			payload["result"] = 0; // Success
			payload["myContactData"] = contactDb->get(cid);
			if (!payload["myContactData"].isNull()) {
				payload["myContactData"].removeMember("ipAddress");
				payload["myContactData"].removeMember("login");
				payload["myContactData"].removeMember("password");
				payload["myContactData"].removeMember("passwordAnswer");
				payload["myContactData"].removeMember("passwordQuestion");
			}
			cout << "handleLogin()/From[" << from << "]OK:" << payload << endl;
			result = true;
		} else {
			// Wrong password
			payload["result"] = 2; // Fail 
			cout << "handleLogin()/From[" << from << "]/FAIL: Wrong password" << endl;

		}
	} else {
		// Not registered
		payload["result"] = 1; // Fail 
		cout << "handleLogin()/From[" << from << "]/FAIL:No contact data to login" << endl;
	}
	/*
		result 0 : SUCCESS
		result 1 : FAILED (NOT REGISTERED)
		result 2 : FAILED (WRONG PASSWORD)
	*/
	sessionControl->sendData(102, payload, from);
	if (result == true) {
	    handleGetAllContact(from);
		Json::Value data;
		data["cid"] = cid;
		handleGetAllConference(data, from);
		return cid;
	} else {
		return "";
	}
	
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
	Json::Value requestMyContactList = data["myContactList"];
	Json::Value updateMyContactList;
	if (requestMyContactList.empty()) {
		contactDb->remove(cid, "myContactList");
	}
	if (!cid.empty() && !requestMyContactList.empty()) {
		contactDb->remove(cid, "myContactList");
		for (int i = 0; i < requestMyContactList.size(); i++) {
			if (!contactDb->search("cid", requestMyContactList[i].asString()).empty() ) {
				updateMyContactList.append(requestMyContactList[i]);
			}
		}		
		if (contactDb->update(cid, "myContactList", updateMyContactList)) {
			std::cout << "handleUpdateMyContactList()/OK:" << contactDb->get(cid, "myContactList") << std::endl;
			handleGetAllContact(from);
		} else {
			std::cout << "handleUpdateMyContactList()/FAILED:" << updateMyContactList << std::endl;
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

void AccountManager::handleUpdateMyContact(Json::Value data, string from)
{
	Json::Value payload;
	string cid = data["cid"].asString();
	string newEmail = data["email"].asString();
	string newName = data["name"].asString();
	if (cid.empty() || contactDb->search("cid", cid).empty()) {
		cout << "handleUpdateMyContact()/FAIL:No CID exists:" << data << endl;
		return;
	}	
	if (contactDb->search("email", newEmail) == "") {
		contactDb->update( cid, "email", newEmail);
		contactDb->update( cid, "name", newName);
		cout << "handleUpdateMyContact()/OK:" << data << endl;
		handleGetAllContact(from);
	} else {
		cout << "handleUpdateMyContact()/FAIL:Same email exists:" << data << endl;
	}		
}

void AccountManager::handleCreateConference(Json::Value data, string from)
{
	Json::Value cidData;
	cidData["cid"] = from;
	string cid = cidData["cid"].asString();
	handleGetAllConference(cidData, from);
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

void AccountManager::handleGetAllConference(Json::Value data, string from)
{
	string cid = data["cid"].asString();
	Json::Value payload;
	Json::Value allConferences = conferenceDb->get();
	for (int i = 0; i < allConferences.size(); i++) {		
		for (int j = 0; j < allConferences[i]["participants"].size(); j++) {
			if (allConferences[i]["participants"][j].asString().compare(cid) == 0) {
				payload.append(allConferences[i]);
				break;
			}
		}
	}
	std::cout << "handleGetAllConference()/From[" << from << "]OK:" << payload << std::endl;	
	sessionControl->sendData(205, payload, from);
}

