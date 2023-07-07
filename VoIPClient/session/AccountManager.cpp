#include <iostream>
#include <thread>
#include <chrono>
#include <string>
//MD5
#include "../../openssl/md5.h"
#include "../../openssl/evp.h"
#include <iomanip>

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

std::string AccountManager::md5(std::string& data) {
	EVP_MD_CTX* mdctx;
	const EVP_MD* md;
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;

	mdctx = EVP_MD_CTX_new();
	md = EVP_md5();
	EVP_DigestInit_ex(mdctx, md, nullptr);

	EVP_DigestUpdate(mdctx, data.c_str(), data.length());

	EVP_DigestFinal_ex(mdctx, md_value, &md_len);

	std::stringstream ss;
	for (unsigned int i = 0; i < md_len; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md_value[i]);
	}

	EVP_MD_CTX_free(mdctx);

	return ss.str();
}

bool AccountManager::isSubstring(const std::string& source, const std::string& target)
{
	return source.find(target) != std::string::npos;
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
	payload["password"] = md5(pw);
	payload["passwordQuestion"] = pwdQuestion;
	payload["passwordAnswer"] = pwdAnswer;

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
	payload["password"] = md5(pw);

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
    // Clear my local data
	AccountManager::myCid = "";
	AccountManager::myConferenceDataList.clear();
	AccountManager::myContactDataList.clear();
	sessionControl->sendData(jsonCString);
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
	payload["password"] = md5(newPW);
	payload["passwordQuestion"] = pwdQ;
	payload["passwordAnswer"] = pwdA;

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

void AccountManager::createConference(long dateAndTime, long duration, std::list<std::string>& participants) {

	Json::Value payload;
	payload["dateAndTime"] = (Json::UInt64)dateAndTime;
	payload["duration"] = (Json::UInt64)duration;
	int index = 0;
	for (const auto& element : participants) {
		payload["participants"][index++] = element;
	}
	Json::Value root;
	root["msgId"] = 206;
	root["payload"] = payload;
	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);
	sessionControl->sendData(jsonString.c_str());
}

void AccountManager::getAllConference(std::string cid)
{
	Json::Value root;
	Json::Value payload;
	payload["cid"] = cid;
	root["msgId"] = 205;
	root["payload"] = payload;
	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);
	sessionControl->sendData(jsonString.c_str());
}

std::list<AccountManager::ContactData> AccountManager::getMyContactList()
{
	//Returns the contactdata list found in the allcontact list based on the cid of mylist
	std::list<ContactData> reList;
	for (auto& myContact : AccountManager::myContactDataList) {
		for (auto& contact : AccountManager::allConatactDataList) {
			if (myContact == contact.cid) {
				reList.push_back(contact);
				break;
			}
		}
	}
	return reList;
}

std::list<AccountManager::ContactData> AccountManager::searchContact(std::string key)
{
	//Find and return list in allContactlist
	std::list<AccountManager::ContactData> reList;

	for (auto& contact : AccountManager::allConatactDataList) {
		if (isSubstring(contact.cid, key)) {
			if (myCid == contact.cid) continue;
			reList.push_back(contact);
			continue;
		}
		if (isSubstring(contact.email, key)) {
			if (myCid == contact.cid) continue;
			reList.push_back(contact);
			continue;
		}
		if (isSubstring(contact.name, key)) {
			if (myCid == contact.cid) continue;
			reList.push_back(contact);
			continue;
		}
	}
	
	return reList;
}

void AccountManager::deleteContact(std::string cid)
{
	//Request updateMyContactList after remove cid contact info in myContactDataList
	for (auto& contact : AccountManager::myContactDataList) {
		//contact is cid
		if (contact == cid) {
			myContactDataList.remove(contact);
			break;
		}
	}

	updateMyContactList(myCid, myContactDataList);
}

void AccountManager::addContact(std::string cid)
{
	//Add cid in myContactDataList and request updateMyContactList
	for (auto& contact : AccountManager::allConatactDataList) {
		if (cid == contact.cid) {
			myContactDataList.push_back(cid);
			break;
		}
	}	
	updateMyContactList(myCid, myContactDataList);
}

void AccountManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

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
			//This contact value is cid
			myContactListResult.push_back(contact.asString());
		}
	}

	AccountManager::myContactDataList = myContactListResult;
	AccountManager::myCid = cid;

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

void AccountManager::updateMyContact(std::string cid, std::string email, std::string name)
{
	Json::Value root;
	root["msgId"] = 107;

	Json::Value payload;
	payload["cid"] = cid;
	payload["email"] = email;
	payload["name"] = name;
	root["payload"] = payload;
	Json::StreamWriterBuilder writerBuilder;
	std::string jsonString = Json::writeString(writerBuilder, root);
	const char* jsonCString = jsonString.c_str();
	sessionControl->sendData(jsonCString);
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

	AccountManager::allConatactDataList = contactDataList;
	//TODO : SEND msg to UI
	for (const auto& element : contactDataList) {
		std::cout << "cid : " << element.cid << " ";
		std::cout << "email : " << element.email << " ";
		std::cout << "name : " << element.name << " ";
		std::cout << std::endl;
	}
}

void AccountManager::handleGetAllMyConference(Json::Value data)
{
	std::list<ConferenceData> conferenceDataList;
	Json::Value payload = data;
	if (payload.isArray()) {
		for (const auto& item : payload) {
			ConferenceData conference;
			conference.rid = item["rid"].asString();
			conference.dataAndTime = item["dateAndTime"].asUInt64();
			conference.duration = item["duration"].asUInt64();
			for (int i = 0; i < item["participants"].size(); i++) {
				conference.participants.push_back(item["participants"][i].asString());
			}
			conferenceDataList.push_back(conference);
		}
	}
	AccountManager::myConferenceDataList = conferenceDataList;
	//TODO : SEND msg to UI
	for (const auto& element : conferenceDataList) {
		std::cout << "rid : " << element.rid << " ";
		std::cout << "dataAndTime : " << element.dataAndTime << " ";
		std::cout << "duration : " << element.duration << " ";		
		for (const auto& item : element.participants) {
			std::cout << "participants : " << item << " ";
		}		
		std::cout << std::endl;
	}

}

// TEMP
void AccountManager::login_() {
	// do login
	std::cout << "ACCOUNT :: doLogin" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("Login");
}

void AccountManager::onLoginSuccess(std::string contactId) {
	std::cout << "[Received] -> onLoginSuccess(): " << contactId << std::endl;
}