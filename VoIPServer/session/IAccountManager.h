#pragma once

#include "SessionControl.h"
#include "../../json/json.h"

class IAccountManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void handleLogin_(std::string requester) = 0;

    // JSON based callback methods
	virtual void handleRegisterContact(Json::Value data, std::string from) = 0;
	virtual std::string handleLogin(Json::Value data, std::string ipAddress, std::string from) = 0;
	virtual bool handleLogout(Json::Value data) = 0;
	virtual void handleUpdateMyContactList(Json::Value data, std::string from) = 0;
	virtual void handleResetPassword(Json::Value data, std::string from) = 0;
	virtual void handleGetAllContact(std::string from) = 0;
	virtual void handleGetAllConference(Json::Value data, std::string from) = 0;
};