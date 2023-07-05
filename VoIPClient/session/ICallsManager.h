#pragma once

#include <iostream>

#include "SessionControl.h"

class ICallsManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void onIncomingCall(std::string connId, std::string from) = 0;
	virtual void onSuccessfulOutgoingCall(std::string connId) = 0;
	virtual void onSuccessfulIncomingCall() = 0;
	virtual void onFailedOutgoingCall(std::string cause) = 0;
	virtual void onRejectedIncomingCall() = 0;
	virtual void onDisconnected() = 0;
};