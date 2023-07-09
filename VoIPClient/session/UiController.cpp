#include "UiController.h"
#include "SessionManager.h"
#include <thread>
#include "../CommandLineInterface.h"
#include <algorithm>
using namespace std;

UiController* UiController::instance = nullptr;
UiController::UiController()
{
	cout << "UiController()" << endl;	
	accountManager = AccountManager::getInstance();
	callsManager = CallsManager::getInstance();

	accountManager->setUiControl(this);
	callsManager->setUiControl(this);
}

UiController* UiController::getInstance() {
	if (instance == nullptr) {
		instance = new UiController();
	}
	return instance;
}

void UiController::releaseInstance()
{
	callsManager->releaseInstance();
	accountManager->releaseInstance();
}

void UiController::setCallbackWnd(CWnd* wnd)
{
	callbackWnds.push_back(wnd);
	callbackWnds.erase(unique(callbackWnds.begin(), callbackWnds.end()), callbackWnds.end());
}

void UiController::postMessage(WPARAM wPram, LPARAM lParam)
{
	callbackWnds.erase(remove_if(callbackWnds.begin(), callbackWnds.end(), [](CWnd* p) {return (p->m_hWnd == 0);}), callbackWnds.end());
	for (int i = 0; i < callbackWnds.size(); i++) {
		if (callbackWnds[i]->m_hWnd != 0) {
			callbackWnds[i]->PostMessageW(UWM_UI_CONTROLLER, wPram, lParam);
		}
	}
}

void UiController::startConnection( string serverIp )
{
	std::string ip = serverIp;
	int serverPort = 0;

	// SessionManager init
	SessionManager::getInstance()->init(ip.c_str(), serverPort);

	// Start CLI
	CommandLineInterface* cli = CommandLineInterface::getInstance();
	cli->startCli(accountManager, callsManager);
}

void UiController::notify(int type, int result)
{
	postMessage(type, (LPARAM)result);
}

list<ContactData> UiController::get_MyContacts()
{
	return accountManager->getMyContactList();
}

list<ConferenceData> UiController::get_MyConferences()
{
	return accountManager->myConferenceDataList;
}

list<ContactData> UiController::get_SearchResult(string key)
{
	return accountManager->searchContact(key);
}

void UiController::req_Connect(CWnd* wnd, string serverIp)
{
	setCallbackWnd(wnd);
    std::thread commandline(&UiController::startConnection, this, serverIp);
    commandline.detach();
}

void UiController::req_GetAllContacts(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->getAllContact();
}

void UiController::req_GetAllConferences(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->getAllConference(accountManager->myCid);
}

void UiController::req_ResetPassword(CWnd* wnd, string newPw, int pwdQ, string pwdA)
{
	setCallbackWnd(wnd);
	accountManager->resetPassword(accountManager->myCid, newPw, pwdQ, pwdA);
}

void UiController::req_Login(CWnd* wnd, string email, string password)
{
	setCallbackWnd(wnd);
	accountManager->login(email, password);
}

void UiController::req_Logout(CWnd* wnd)
{
	setCallbackWnd(wnd);
	accountManager->logout(accountManager->myCid);
}

void UiController::req_Register(CWnd* wnd, string id, string email, string pw, string name, int pwdQ, string pwdA)
{
	setCallbackWnd(wnd);
	accountManager->registerAccount(id, email, pw, name, pwdQ, pwdA);
}

void UiController::req_AddMyContact(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	accountManager->addContact(id);
}

void UiController::req_DeleteMyContact(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	accountManager->deleteContact(id);
}

void UiController::request_OutgoingCall(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	callsManager->startOutgoingCall(id);
}

void UiController::request_AnswerCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->answerCall();
}

void UiController::request_RejectCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->rejectCall();
}

void UiController::request_EndCall(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->disconnectCall();
}

void UiController::request_CreateConference(CWnd* wnd, long time, long duration, list<string>& list)
{
	setCallbackWnd(wnd);
	accountManager->createConference(time, duration, list);
}

void UiController::request_JoinConference(CWnd* wnd, string id)
{
	setCallbackWnd(wnd);
	callsManager->joinConference(id);
}

void UiController::request_ExitConference(CWnd* wnd)
{
	setCallbackWnd(wnd);
	callsManager->exitConference();
}

