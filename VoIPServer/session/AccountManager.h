#pragma once

#include <string>

#include "AccountManagerListener.h"
#include "SessionControl.h"

class AccountManager : public AccountManagerListener {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;

	AccountManager();

public:
	static AccountManager* getInstance();

	void setSessionControl(SessionControl* control);

	// Listener
	void handleLogin(std::string requester) override;
};