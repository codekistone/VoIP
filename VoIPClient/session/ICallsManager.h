#pragma once

#include <iostream>

#include "SessionControl.h"
#include "../../json/json.h"

class ICallsManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void onOutgoingCallResult(Json::Value data) = 0;
	virtual void onIncomingCall(Json::Value data) = 0;
	virtual void onIncomingCallResult(Json::Value data) = 0;
	virtual void onDisconnected(Json::Value data) = 0;
	virtual void onJoinConferenceResult(Json::Value data) = 0;
	virtual void onExitConference(Json::Value data) = 0;
	virtual void onVideoQualityChanged(Json::Value data) = 0;
};