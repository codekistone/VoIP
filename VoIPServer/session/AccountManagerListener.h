#pragma once

#include <string>

class AccountManagerListener {
public:
	virtual void handleLogin(std::string requester) = 0;
};