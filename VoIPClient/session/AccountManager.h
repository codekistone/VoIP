#pragma once

#include "IAccountManager.h"
#include "SessionControl.h"
#include "../../json/json.h"

class AccountManager : public IAccountManager {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;

	AccountManager();

public:
	static AccountManager* getInstance();
	static void releaseInstance();

	void login();

	// Listener
	void setSessionControl(SessionControl* control) override;
	void onLoginSuccess(std::string contactId) override;
};
