#pragma once

#include <iostream>

#include "SessionControl.h"
#include "../../json/json.h"
#include "IUiController.h"

class IAccountManager {
public:

	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void setUiControl(IUiController* control) = 0;

	virtual void handleConnect(int result) = 0;
	virtual void handleRegisterContact(Json::Value data) = 0;
	virtual void handleLogin(Json::Value data) = 0;
	virtual void handleResetPassword(Json::Value data) = 0;	
	virtual void handleGetAllContact(Json::Value data) = 0;
	virtual void handleGetAllMyConference(Json::Value data) = 0;
};