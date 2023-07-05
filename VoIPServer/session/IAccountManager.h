#pragma once

#include "SessionControl.h"
#include "../../json/json.h"

class IAccountManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void handleLogin_(std::string requester) = 0;

    // JSON based callback methods
	virtual void handleRegisterContact(Json::Value data) = 0;
	virtual void handleLogin(Json::Value data) = 0;	
	virtual void handleLogout(Json::Value data) = 0;
	virtual void handleUpdateMyContactList(Json::Value data) = 0;
	virtual void handleResetPassword(Json::Value data) = 0;
	virtual void handleGetAllContact() = 0;
};