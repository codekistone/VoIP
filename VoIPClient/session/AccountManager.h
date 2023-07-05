#pragma once

#include "AccountManagerListener.h"
#include "SessionControl.h"
#include "../../json/json.h"

class AccountManager : public AccountManagerListener {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;

	AccountManager();

public:
	static AccountManager* getInstance();

	void setSessionControl(SessionControl* control);
	void login();

	// Listener
	void onLoginSuccess(std::string contactId) override;
};
