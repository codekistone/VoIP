#pragma once

#include <string>

#include "IAccountManager.h"
#include "SessionControl.h"
#include "ContactDb.h"
#include "ConferenceDb.h"

class AccountManager : public IAccountManager {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;
	ContactDb* contactDb; // Contact Database
	ConferenceDb* conferenceDb; // Conference Database

	AccountManager();

public:
	static AccountManager* getInstance();
	static void releaseInstance();

	void release();

	// interface
	void setSessionControl(SessionControl* control) override;

    // JSON based callback methods
	void handleRegisterContact(Json::Value data, string from) override;
	string handleLogin(Json::Value data, string ipAddress, string from) override;
	bool handleLogout(Json::Value data) override;
	void handleUpdateMyContactList(Json::Value data, string from) override;
	void handleResetPassword(Json::Value data, string from) override;
	void handleUpdateMyContact(Json::Value data, string from) override;
	void handleCreateConference(Json::Value data, string from) override;
	void handleGetAllContact(string from) override;
	void handleGetAllConference(Json::Value data, string from) override;
};