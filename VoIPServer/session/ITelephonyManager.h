#pragma once

#include "SessionControl.h"
#include "../../json/json.h"

class ITelephonyManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void handleOutgoingCall(Json::Value data) = 0;
	virtual void handleOutgoingCallNoUser(Json::Value data) = 0;
	virtual void handleIncomingCallResponse(Json::Value data) = 0;
	virtual void handleDisconnect(Json::Value data) = 0;
	virtual void releaseConnection(std::string cid) = 0;
};