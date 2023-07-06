

#include <iostream>
#include <thread>

#include "CommandLineInterface.h"
#include "session/SessionControl.h"
#include "session/SessionManager.h"
#include "dirent.h"
#include "../json/json.h"

using namespace std;
#define MAX_JSON_FILE_SIZE		40960
#define JSON_RELATIVE_DIR		"../json_files/"

CommandLineInterface* CommandLineInterface::instance = nullptr;

char readBuffer[MAX_JSON_FILE_SIZE] = { 0, };
std::map<int, string> jsonFiles;

bool openDirectory(string path) {
	struct dirent* ent;
	DIR* dir = opendir(path.c_str());
	if (dir != NULL) {
		int index = 0;
		while ((ent = readdir(dir)) != NULL) {
			string fileName = ent->d_name;
			if (fileName.find("json") != std::string::npos) {
				jsonFiles.insert({ index++, ent->d_name });
			}
		}
	}
	closedir(dir);
	if (jsonFiles.size() > 0) {
		return true;
	}
	else {
		return false;
	}
}


Json::Value openJson(string path, string name) {
	string fileName = path + name;
	FILE* fp = nullptr;
	fopen_s(&fp, &*fileName.begin(), "rb");
	if (fp == nullptr) {
		return NULL;
	}
	size_t fileSize = fread(readBuffer, 1, MAX_JSON_FILE_SIZE, fp);
	fclose(fp);
	std::string config_doc = readBuffer;
	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(config_doc, root);
	if (parsingSuccessful == false) {
		return NULL;
	}
	return root;
}

CommandLineInterface::CommandLineInterface() {
	sessionManager = SessionManager::getInstance();
	accountManager = AccountManager::getInstance();
	callsManager = CallsManager::getInstance();
}

CommandLineInterface* CommandLineInterface::getInstance() {
	if (instance == nullptr) {
		instance = new CommandLineInterface();
	}
	return instance;
}

void CommandLineInterface::startJsonCli()
{
	cout << "======================================================" << endl;
	cout << "COMMAND LINE INTERFACE " << std::endl;
	cout << "======================================================" << endl;
	std::string serverIp, serverPort;
	std::cout << "Input serverIP(127.0.0.1): ";
	getline(std::cin, serverIp);
	std::cout << "Input serverPort(5555): ";
	getline(std::cin, serverPort);

	int port = serverPort.length() > 0 ? std::stoi(serverPort) : 0;
	std::thread t(&SessionManager::init, sessionManager, serverIp.c_str(), port);
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); //TEST

	string jsonFilesPath = JSON_RELATIVE_DIR;
	if (!openDirectory(jsonFilesPath)) {
		jsonFilesPath = "../" + jsonFilesPath;
		openDirectory(jsonFilesPath);
	}
	while (true) {
		Json::FastWriter fastWriter;
		string input;		
		cout << "======================================================" << endl;
		cout << "JSON CLI" << endl;
		cout << "======================================================" << endl;
		for (int i = 0; i < jsonFiles.size(); i++) {
			cout << i << ".  " << jsonFiles[i] << endl;
		}
		cout << "======================================================" << endl;
		cout << "ENTER : ";		
		getline(cin, input);
		if (!input.empty()) {
			int number = stoi(input);
			string fileName = jsonFiles[number];
			if (!fileName.empty()) {
				Json::Value jsonData = openJson(jsonFilesPath, fileName);
				cout << "--------------------------------------------------------" << endl;
				cout << "SEND : " << jsonData << endl;
				cout << "--------------------------------------------------------" << endl;
				std::string jsonDataString = fastWriter.write(jsonData);
				int retValue = sessionManager->sendData(jsonDataString.c_str());
			}
		}
	}
	t.join();
	cout << "JOIN CALLED" << endl;
}

void CommandLineInterface::startCommandCli()
{
	cout << "======================================================" << endl;
	cout << "COMMAND CLI" << std::endl;
	cout << "======================================================" << endl;
	std::string serverIp, serverPort;
	std::cout << "Input serverIP(127.0.0.1): ";
	getline(std::cin, serverIp);
	std::cout << "input serverPort(5555): ";
	getline(std::cin, serverPort);
	int port = serverPort.length() > 0 ? std::stoi(serverPort) : 0;

	std::thread t(&SessionManager::init, sessionManager, serverIp.c_str(), port);
	std::this_thread::sleep_for(std::chrono::milliseconds(300)); 

	string num;
	std::string id, email, pw, name, pwdAnswer, pwdQuestion, newpw;
	std::string inputID, inputPW, input;
	std::list<AccountManager::ContactData> list;

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(300)); 
		cout << "======================================================" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << " ACCOUNT" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << " 1. REGISTER_CONTACT" << endl;
		cout << " 2. LOGIN" << endl;
		cout << " 3. LOGOUT" << endl;
		cout << " 4. UPDATE_MY_CONTACTLIST" << endl;
		cout << " 5. RESET_PWORD" << endl;
		cout << " 6. GET_ALL_CONTACT" << endl;
		cout << " 7. GETMYCONTACTLIST FOR UI" << endl;
		cout << " 8. SERCHCONTACT FOR UI" << endl;
		cout << " 9. ADDCONTACT IN MYCONTACTLIST" << endl;
		cout << "10. DELETECONTACT IN MYCONTACTLIST" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << " CALL" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "11. ANSWER CALL " << endl;
		cout << "12. OUTGOING CALL " << endl;
		cout << "13. REJECT CALL " << endl;
		cout << "14. DISCONNECT CALL " << endl;
		cout << "------------------------------------------------------" << endl;
		cout << " CONFERENCE" << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "15. GET ALL CONFERENCES " << endl;
		cout << "16. CREATE CONFERENCE " << endl;
		cout << "17. JOIN CONFERENCE " << endl;
		cout << "18. EXIT_CONFERENCE " << endl;
		cout << "======================================================" << endl;
		std::cout << "Input int : " ;
		getline(cin, num);
		int selected = stoi(num);
		if (selected == 0) {
			continue;
		}
		switch (selected) {
		case 1: //REGISTER
			//Register
			std::cout << "cid : ";
			getline(std::cin >> std::ws, id);
			std::cout << "email : ";
			getline(std::cin >> std::ws, email);
			std::cout << "password : ";
			getline(std::cin >> std::ws, pw);
			std::cout << "name : ";
			getline(std::cin >> std::ws, name);
			std::cout << "pwdQuestion : ";
			getline(std::cin >> std::ws, pwdQuestion);
			std::cout << "pwdAnswer : ";
			getline(std::cin >> std::ws, pwdAnswer);
			accountManager->registerAccount(id, email, pw, name, std::stoi(pwdQuestion), pwdAnswer);
			break;

		case 2: //LOGIN
			std::cout << "cid : ";
			getline(std::cin >> std::ws, inputID);
			std::cout << "password : ";
			getline(std::cin >> std::ws, inputPW);
			accountManager->login(inputID, inputPW);
			break;

		case 3: //LOGOUT
			std::cout << "cid : ";
			getline(std::cin >> std::ws, inputID);
			accountManager->logout(inputID);
			break;

		case 4: //UPDATE MY CONTACTLIST
			std::cout << "Processing updateMyContactList test ..." << std::endl;
			//accountManager->updateMyContactList(accountManager->myCid, nullptr);
			break;

		case 5: //RESET PW
			std::cout << std::endl << "Processing reset pw test ..." << std::endl;
			std::cout << "newPassword : ";
			getline(std::cin >> std::ws, newpw);
			accountManager->resetPassword(id, newpw, std::stoi(pwdQuestion), pwdAnswer);
			break;

		case 6: //GET ALL CONTACTLIST FROM SERVER
			std::cout << std::endl << "Processing getAllContact test ..." << std::endl;
			accountManager->getAllContact(id);
			break;

		case 7://GETMYCONTACTLIST FOR UI
			list = accountManager->getMyContactList();
			for (auto& myContact : list) {
				std::cout << "contact : " << myContact.cid << ", " << myContact.email << ", " << myContact.name << std::endl;
			}
			break;
		case 8://SERCHCONTACT FOR UI
			std::cout << std::endl << "Please input search text : ";
			getline(std::cin >> std::ws, input);
			list = accountManager->searchContact(input);
			for (auto& myContact : list) {
				std::cout << myContact.cid << ", " << myContact.email << ", " << myContact.name << std::endl;
			}
			break;
		case 9://ADDCONTACT IN MYCONTACTLIST
			std::cout << std::endl << "Please add contact cid : ";
			getline(std::cin >> std::ws, input);
			accountManager->addContact(input);
			break;
		case 10://DELETECONTACT IN MYCONTACTLIST
			std::cout << std::endl << "Please delete contact cid : ";
			getline(std::cin >> std::ws, input);
			accountManager->deleteContact(input);
			break;
		case 11: // ANSWER CALL
			callsManager->answerCall();
			break;
		case 12: // OUTGOING CALL
			{
				std::cout << "Insert target CID : ";
				string targetCid;
				std::cin >> targetCid;
				std::cin.ignore();
				callsManager->startOutgoingCall(targetCid);
				break;
			}
		case 13: // REJECT CALL
			callsManager->rejectCall();
			break;
		case 14: // DISCONNECT CALL
			callsManager->disconnectCall();
			break;
		case 15: // GET ALL CONFERENCES 
			std::cout << "cid : ";
			getline(std::cin >> std::ws, inputID);
			accountManager->getAllConference(inputID);
			break;
		case 16: // CREATE CONFERENCE 
			{
				string input;
				long time, duration;
				std::cout << "Time : ";
				getline(std::cin >> std::ws, input);
				time = stol(input);
				std::cout << "Duration : ";
				getline(std::cin >> std::ws, input);
				duration = stol(input);
				std::list<std::string> participants;
				while (true) {
					string participant;
					std::cout << "participant (Q to finish) : ";
					getline(std::cin >> std::ws, participant);
					if (participant == "Q") {
						break;
					}else {
						participants.push_back(participant);
					}
				}
				accountManager->createConference(time, duration, participants);
				break;
			}
		case 17: // JOIN CONFERENCE
			std::cout << "rid : ";
			getline(std::cin >> std::ws, inputID);
			callsManager->joinConference(inputID);
			break;
		case 18: // EXIT CONFERENCE
			std::cout << "rid : ";
			getline(std::cin >> std::ws, inputID);
			callsManager->exitConference(inputID);
			// TODO 
			break;
		default:			
			break;
		}		
	}
	t.join();
}
