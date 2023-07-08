#include <iostream>
#include <thread>

#include "CommandLineInterface.h"
#include "session/SessionControl.h"
#include "../json/json.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "session/Constants.h"

CommandLineInterface* CommandLineInterface::instance = nullptr;

static std::string getMyIpAddress(void)
{
	struct addrinfo* _addrinfo;
	struct addrinfo* _res;
	char _address[INET6_ADDRSTRLEN];
	char szHostName[255];
	gethostname(szHostName, sizeof(szHostName));
	getaddrinfo(szHostName, NULL, 0, &_addrinfo);
	for (_res = _addrinfo; _res != NULL; _res = _res->ai_next){
		if (_res->ai_family == AF_INET) {
			if (NULL != inet_ntop(AF_INET, &((struct sockaddr_in*)_res->ai_addr)->sin_addr,_address, sizeof(_address)) ){
				return _address;
			}
		}
	}
	return "";
}

CommandLineInterface* CommandLineInterface::getInstance() {
	if (instance == nullptr) {
		instance = new CommandLineInterface();
	}
	return instance;
}

void CommandLineInterface::getServerInfo(std::string& ip, int& port)
{
	std::cout << "======================================================" << std::endl;
	std::cout << "Get Server Info " << std::endl;
	std::cout << "======================================================" << std::endl;
	std::string serverIp, serverPort;
	std::cout << "Input serverIP(127.0.0.1): ";
	getline(std::cin, ip);
	std::cout << "input serverPort(5555): ";
	getline(std::cin, serverPort);
	int portInt = serverPort.length() > 0 ? std::stoi(serverPort) : 5555;
	port = portInt;
}

void CommandLineInterface::startCli(AccountManager* accountManager, CallsManager* callsManager)
{
	std::string num;
	std::string id, email, pw, name, pwdAnswer, pwdQuestion, newpw;
	std::string inputID, inputPW, input;
	std::list<AccountManager::ContactData> list;

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		bool inCall = isInCall();
		if (inCall) {
			printCallState();
		}
		std::cout << "======================================================" << std::endl;
		if (!inCall) {
			std::cout << " 1. REGISTER CONTACT" << std::endl;
			std::cout << " 2. LOG IN" << std::endl;
			std::cout << " 3. LOG OUT" << std::endl;
			std::cout << " 5. RESET PWORD" << std::endl << std::endl;

			std::cout << " 10. UPDATE MY CONTACT" << std::endl;
			std::cout << " 11. GET ALL CONTACT" << std::endl;
			std::cout << " 12. GET MY CONTACT LIST FOR UI" << std::endl;
			std::cout << " 13. SERCH CONTACT FOR UI" << std::endl;
			std::cout << " 14. ADD CONTACT IN MYCONTACTLIST" << std::endl;
			std::cout << " 15. DELETE CONTACT FROM MYCONTACTLIST" << std::endl << std::endl;
		}
		std::cout << " 20. GET CALL_STATE" << std::endl;
		std::cout << " 21. ANSWER CALL " << std::endl;
		std::cout << " 22. OUTGOING CALL " << std::endl;
		std::cout << " 23. REJECT CALL " << std::endl;
		std::cout << " 24. DISCONNECT CALL " << std::endl << std::endl;

		std::cout << " 30. GET MY CONFERENCES " << std::endl;
		std::cout << " 31. CREATE CONFERENCE " << std::endl;
		std::cout << " 32. JOIN CONFERENCE " << std::endl;
		std::cout << " 33. EXIT_CONFERENCE " << std::endl << std::endl;
		std::cout << " << MY DATA (" << getMyIpAddress() << ") >> " << std::endl;
		
		if (accountManager->myCid.empty()) {
			std::cout << "My CID : (EMPTY)" << std::endl;
		} else {
			std::cout << "My CID : " << accountManager->myCid << std::endl;
		}		
		if (accountManager->myContactDataList.empty()) {
			std::cout << "My ContactList : (EMPTY)";
		} else {
			std::cout << "My ContactList : ";
			for (const auto& item : accountManager->myContactDataList) {
				std::cout << item << " ";
			}
		}
		std::cout << std::endl;
		if ( accountManager->myConferenceDataList.empty() ) {
			std::cout << "My ConferenceList : (EMPTY)" << std::endl;
		}
		else {
			std::cout << "My ConferenceList : " << std::endl;
			for (const auto& item : accountManager->myConferenceDataList) {
				std::cout << "- id[" << item.rid << "]/";
				std::cout << "time[" << item.dataAndTime << "]/";
				std::cout << "dur[" << item.duration << "]/";
				std::list<std::string> li = item.participants;
				std::list<std::string>::iterator it;
				std::cout << "part[";
				for (it = li.begin(); it != li.end(); it++) {
					std::cout << *it << " ";
				}
				std::cout << "]";
				std::cout << std::endl;
			}
		}		
		std::cout << "======================================================" << std::endl;
		std::cout << "Choose (Q to Exit): " ;
		getline(std::cin, num);
		int selected = 0;
		try {
			selected = stoi(num);
			if (selected == 0) {
				continue;
			}
			else if (isInCall() && selected < 20) {
				continue;
			}
		}
		catch (std::exception ex) {
			// Not number
			if (num == "Q") {
				break;
			}
			continue;
		}
		switch (selected) {
		case 1: //REGISTER
			//Register
			std::cout << "REGISTER CONTACT" << std::endl;
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
			std::cout << "LOG IN" << std::endl;
			std::cout << "email : ";
			getline(std::cin >> std::ws, inputID);
			std::cout << "password : ";
			getline(std::cin >> std::ws, inputPW);
			accountManager->login(inputID, inputPW);
			break;

		case 3: //LOGOUT
			std::cout << "LOG OUT" << std::endl;
			if (!accountManager->myCid.empty()) {
				accountManager->logout(accountManager->myCid);
			}
			break;

		case 5: //RESET PW
			std::cout << "RESET PWORD" << std::endl;
			std::cout << "cid : ";
			getline(std::cin >> std::ws, id);
			std::cout << "newPassword : ";
			getline(std::cin >> std::ws, newpw);
			std::cout << "prev passwordQuestion : ";
			getline(std::cin >> std::ws, pwdQuestion);
			std::cout << "prev passwordAnswer : ";
			getline(std::cin >> std::ws, pwdAnswer);
			accountManager->resetPassword(id, newpw, std::stoi(pwdQuestion), pwdAnswer);
			break;
		case 10: // UPDATE MY CONTACT
			if (!accountManager->myCid.empty()) {
				std::cout << "email : ";
				getline(std::cin >> std::ws, email);
				std::cout << "name : ";
				getline(std::cin >> std::ws, name);
				accountManager->updateMyContact(accountManager->myCid, email, name);
			}
			break;
		case 11: //GET ALL CONTACTLIST FROM SERVER
			std::cout << "GET ALL CONTACT" << std::endl;
			accountManager->getAllContact();
			break;

		case 12://GETMYCONTACTLIST FOR UI
			std::cout << "GET MY CONTACT LIST FOR UI" << std::endl;
			list = accountManager->getMyContactList();
			for (auto& myContact : list) {
				std::cout << "contact : " << myContact.cid << ", " << myContact.email << ", " << myContact.name << std::endl;
			}
			break;
		case 13://SERCHCONTACT FOR UI
			std::cout << "SEARCH CONTACT FOR UI" << std::endl;
			std::cout << "Keyword : ";
			getline(std::cin >> std::ws, input);
			list = accountManager->searchContact(input);
			for (auto& myContact : list) {
				std::cout << myContact.cid << ", " << myContact.email << ", " << myContact.name << std::endl;
			}
			break;
		case 14://ADDCONTACT IN MYCONTACTLIST
			std::cout << "ADD CONTACT IN MYCONTACTLIST" << std::endl;
			std::cout << "cid : ";
			getline(std::cin >> std::ws, input);
			accountManager->addContact(input);
			break;
		case 15://DELETECONTACT IN MYCONTACTLIST
			std::cout << "DELETE CONTACT FROM MYCONTACTLIST" << std::endl;
			std::cout << "cid : ";
			getline(std::cin >> std::ws, input);
			accountManager->deleteContact(input);
			break;
		case 20: // GET CALL_STATE
			printCallState();
			break;
		case 21: // ANSWER CALL
			std::cout << "ANSWER CALL" << std::endl;
			callsManager->answerCall();
			break;
		case 22: // OUTGOING CALL
			{
				std::cout << "OUTGOING CALL" << std::endl;
				std::cout << "cid : ";
				std::string targetCid;
				std::cin >> targetCid;
				std::cin.ignore();
				callsManager->startOutgoingCall(targetCid);
				break;
			}
		case 23: // REJECT CALL
			std::cout << "REJECT CALL" << std::endl;
			callsManager->rejectCall();
			break;
		case 24: // DISCONNECT CALL
			std::cout << "DISCONNECT CALL" << std::endl;
			callsManager->disconnectCall();
			break;
		case 30: // GET MY CONFERENCES 
			std::cout << "GET MY CONFERENCES" << std::endl;
			accountManager->getAllConference(accountManager->myCid);
			break;
		case 31: // CREATE CONFERENCE 
			{
				std::cout << "CREATE CONFERENCE" << std::endl;
				std::string input;
				long time, duration;
				std::cout << "Time(now: ENTER) : ";
				getline(std::cin, input);
				if (input.empty()) {
					std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
					std::chrono::system_clock::duration duration = now.time_since_epoch();
					time = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
				}
				else {
					time = stol(input);
				}
				std::cout << "Duration(second) : ";
				getline(std::cin >> std::ws, input);
				duration = stol(input);
				std::list<std::string> participants;
				while (true) {
					std::string participant;
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
		case 32: // JOIN CONFERENCE
			std::cout << "JOIN CONFERENCE" << std::endl;
			std::cout << "rid : ";
			getline(std::cin >> std::ws, inputID);
			callsManager->joinConference(inputID);
			break;
		case 33: // EXIT CONFERENCE
			std::cout << "EXIT CONFERENCE" << std::endl;
			callsManager->exitConference();
			break;
		default:			
			break;
		}		
	}
	std::cout << "CLI Terminated " << std::endl;
	if (!accountManager->myCid.empty()) {
		accountManager->logout(accountManager->myCid);
	}
}

bool CommandLineInterface::isInCall() {
	Call* call = CallsManager::getInstance()->getCall();
	if (call == nullptr) {
		return false;
	}
	int state = call->getCallState();
	return state != CallState::STATE_IDLE && state != CallState::STATE_DISCONNECTED;
}

void CommandLineInterface::printCallState() {
	std::string callId;
	int state = 0;
	std::string displayState;
	Call* call = CallsManager::getInstance()->getCall();
	if (call == nullptr) {
		callId = "NO_CALL";
	}
	else {
		callId = call->getCallId();
		state = call->getCallState();
	}

	switch (state)
	{
	case 1:
		displayState = "STATE_DIALING";
		break;
	case 2:
		displayState = "STATE_RINGING";
		break;
	case 4:
		displayState = "STATE_ACTIVE";
		break;
	case 7:
		displayState = "STATE_DISCONNECTED";
		break;
	case 8:
		displayState = "STATE_IDLE";
		break;
	default:
		break;
	}

	std::cout << std::endl;
	std::cout << "<< CALL INFO (" << callId << "): " << displayState << " >> " << std::endl;
}