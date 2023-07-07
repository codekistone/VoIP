#pragma once

#include <iostream>

#include "SessionControl.h"
#include "../../json/json.h"

class IAccountManager {
public:

	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void onLoginSuccess(std::string contactId) = 0;
	virtual void handleRegisterContact(Json::Value data) = 0;
	virtual void handleLogin(Json::Value data) = 0;
	virtual void handleResetPassword(Json::Value data) = 0;	
	virtual void handleGetAllContact(Json::Value data) = 0;
	virtual void handleGetAllMyConference(Json::Value data) = 0;
};