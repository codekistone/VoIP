#pragma once

#define _AFXDLL

#include <afxwin.h>
#include "AccountManager.h"
#include "CallsManager.h"
#include "SessionManager.h"
#include "Data.h"
#include "IUiController.h"

using namespace std;

class UiController : public IUiController
{
private:
	UiController();
	static UiController* instance;

	// UI --> ManagersManagers 
	AccountManager* accountManager;
	CallsManager* callsManager;

	// Managers --> UI
	// MainFrame window to send response message
	vector<CWnd*> callbackWnds;

public:	
	static UiController* getInstance();
	void releaseInstance();
	void setCallbackWnd(CWnd* wnd);
	void postMessage(WPARAM wPram, LPARAM lParam);
	void startConnection( string serverIp );

	// Interface method
	void notify(int type, int result) override;

	//-----------------------------------------
	// Functions to get data from Managers
	list<ContactData> get_MyContacts();
	list<ContactData> get_SearchResult(string key);
	list<ConferenceData> get_MyConferences();

	//-----------------------------------------
	// Functions to request server
	void req_Connect(CWnd* wnd, string serverIp);
	void req_GetAllContacts(CWnd* wnd);
	void req_GetAllConferences(CWnd* wnd);
	void req_ResetPassword(CWnd* wnd, string newPw, int pwdQ, string pwdA);
	void req_Login(CWnd* wnd, string email, string password);
	void req_Logout(CWnd* wnd);
	void req_Register(CWnd* wnd, string id, string email, string pw, string name, int pwdQ, string pwdA);
	void req_AddMyContact(CWnd* wnd, string id);
	void req_DeleteMyContact(CWnd* wnd, string id);

	void request_OutgoingCall(CWnd* wnd, string id);
	void request_AnswerCall(CWnd* wnd );
	void request_RejectCall(CWnd* wnd );
	void request_EndCall(CWnd* wnd );

	void request_CreateConference(CWnd* wnd, long time, long duration, list<string>& list);
	void request_JoinConference(CWnd* wnd, string id);
	void request_ExitConference(CWnd* wnd);
};