#pragma once

#include "../../json/json.h"

class AccountManagerListener {
public:
	virtual void handleLogin_(std::string requester) = 0;

    // JSON based callback methods
	virtual void handleRegisterContact(Json::Value data) = 0;
	virtual void handleLogin(Json::Value data) = 0;	
	virtual void handleLogout(Json::Value data) = 0;
	virtual void handleUpdateMyContactList(Json::Value data) = 0;
	virtual void handleResetPassword(Json::Value data) = 0;
	virtual void handleGetAllContact() = 0;
};