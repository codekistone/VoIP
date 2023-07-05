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

void AccountManager::releaseInstance() {
	if (instance != nullptr) {
		instance->setSessionControl(nullptr);
		delete instance;
		instance = nullptr;
		std::cout << "AccountManager::releaseInstance" << std::endl;
	}
}

void AccountManager::login_() {
	// do login
	std::cout << "ACCOUNT :: doLogin" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("Login");
}

void AccountManager::registerAccount(std::string id, std::string email, std::string pw, std::string name, int pwdQuestion, std::string pwdAnswer)
{
	Json::Value root;
	root["msgId"] = 101;
	Json::Value payload;

	if (id.empty()) {
		payload["cid"] = email;
	}
	else {
		payload["cid"] = id;
	}
	
	payload["email"] = email;
	payload["name"] = name;
	payload["password"] = pw;
	payload["password_question"] = pwdQuestion;
	payload["password_answer"] = pwdAnswer;

	root["payload"] = payload;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}

void AccountManager::login(std::string email, std::string pw) {

	Json::Value root;
	root["msgId"] = 102;
	Json::Value payload;

	payload["email"] = email;
	payload["password"] = pw;

	root["payload"] = payload;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}

void AccountManager::logout(std::string cid) {

	Json::Value root;
	root["msgId"] = 103;
	Json::Value payload;

	payload["cid"] = cid;

	root["payload"] = payload;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}


// Implement interface
void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void AccountManager::onLoginSuccess(std::string contactId) {
	std::cout << "[Received] -> onLoginSuccess(): " << contactId << std::endl;
}

void AccountManager::updateMyContactList(std::string cid, std::list<std::string>& list) {
	Json::Value root;
	root["msgId"] = 104;

	Json::Value payload;
	Json::Value contactList;

	payload["cid"] = cid;
	for (const auto& element : list) {
		contactList.append(element);
	}

	payload["myContactList"] = contactList;
	root["payload"] = payload;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}

void AccountManager::resetPassword(std::string cid, std::string newPW, int pwdQ, std::string pwdA) {
	Json::Value root;
	root["msgId"] = 105;
	Json::Value payload;

	payload["cid"] = cid;
	payload["password"] = newPW;
	payload["password_question"] = pwdQ;
	payload["password_answer"] = pwdA;

	root["payload"] = payload;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}

void AccountManager::getAllContact(std::string cid) {
	Json::Value root;
	root["msgId"] = 106;

	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);

	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
}


// Implement listener
void AccountManager::handleLogin(Json::Value msg) {
	//std::cout << "[Received] -> onLoginSuccess(): " << contactId << std::endl;
	Json::Value payload = msg;
	std::string cid;
	std::string email;
	std::string name;
	std::list<std::string > myContactListResult;

	int result = payload["result"].asInt();
	Json::Value myContactData = payload["myContactData"];
	cid = myContactData["cid"].asString();
	email = myContactData["email"].asString();
	name = myContactData["name"].asString();
	Json::Value myContactList = myContactData["myContactList"];
	if (myContactList.isArray()) {
		for (const auto& contact : myContactList) {
			//std::string contactCid = contact["cid"].asString();
			//myContactListResult.push_back(contactCid);
			myContactListResult.push_back(contact.asString());
		}
	}
	
	//TODO : SEND msg to UI
	std::cout << "cid: " << cid << std::endl;
	std::cout << "email: " << email << std::endl;
	std::cout << "name: " << name << std::endl;
	for (const auto& element : myContactListResult) {
		std::cout << element << " ";
	}
	std::cout << std::endl;
}

void AccountManager::handleRegisterContact(Json::Value msg) {
	//std::cout << "[Received] -> onLoginSuccess(): " << msg << std::endl;

	int result = 1; //(0:SUCCESS, 1:FAIL)
	std::string reason;

	Json::Value payload = msg;
	
	result = payload["result"].asInt();
	reason = payload["reason"].asString();
		
	//TODO : SEND msg to UI
	if (result == 0) { // SUCCESS
		std::cout << "Register Result : " << result << std::endl;
	}
	else {
		std::cout << "Register Result : " << result << std::endl;
		std::cout << "Register Reason : " << reason << std::endl;

	}
}

void AccountManager::handleResetPassword(Json::Value msg) {
	int result = 1; //(0:SUCCESS, 1:FAIL)

	Json::Value payload = msg;
	result = payload["result"].asInt();
	
	//TODO : SEND msg to UI
	if (result == 0) { // SUCCESS
		std::cout << "Register Result : " << result << std::endl;
	}
	else {
		std::cout << "Register Result : " << result << std::endl;
	}
}

void AccountManager::handleGetAllContact(Json::Value msg) {
	//Don't send myContactList
	std::list<ContactData> contactDataList;
	Json::Value payload = msg;
	if (payload.isArray()) {

		for (const auto& item : payload) {
			ContactData contact;
			contact.cid = item["cid"].asString();
			contact.email = item["email"].asString();
			contact.name = item["name"].asString();
			contactDataList.push_back(contact);
		}
	}

	//TODO : SEND msg to UI
	for (const auto& element : contactDataList) {
		std::cout << "cid : " << element.cid << " ";
		std::cout << "email : " << element.email << " ";
		std::cout << "name : " << element.name << " ";
		std::cout << std::endl;
	}
}