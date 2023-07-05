#pragma once

#include <string>

#include "AccountManagerListener.h"
#include "SessionControl.h"
#include "ContactDb.h"

class AccountManager : public AccountManagerListener {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;
	ContactDb* contactDb; // Contact Database

	AccountManager();

public:
	static AccountManager* getInstance();

	void setSessionControl(SessionControl* control);

	// Listener
	void handleLogin_(std::string requester) override;

    // JSON based callback methods
	void handleRegisterContact(Json::Value data) override;
	void handleLogin(Json::Value data) override;
	void handleLogout(Json::Value data) override;
	void handleUpdateMyContactList(Json::Value data) override;
	void handleResetPassword(Json::Value data) override;
	void handleGetAllContact() override;
};