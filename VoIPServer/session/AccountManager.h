#pragma once

#include <string>

#include "IAccountManager.h"
#include "SessionControl.h"
#include "ContactDb.h"

class AccountManager : public IAccountManager {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;
	ContactDb* contactDb; // Contact Database

	AccountManager();

public:
	static AccountManager* getInstance();
	static void releaseInstance();

	// Listener
	void setSessionControl(SessionControl* control) override;
	void handleLogin_(std::string requester) override;

    // JSON based callback methods
	void handleRegisterContact(Json::Value data, string from) override;
	string handleLogin(Json::Value data, string ipAddress, string from) override;
	bool handleLogout(Json::Value data) override;
	void handleUpdateMyContactList(Json::Value data, string from) override;
	void handleResetPassword(Json::Value data, string from) override;
	void handleGetAllContact(string from) override;
};