#pragma once

class AccountManagerListener {
public:
	virtual void onLoginSuccess(std::string contactId) = 0;
};