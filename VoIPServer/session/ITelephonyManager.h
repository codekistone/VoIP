#pragma once

#include "SessionControl.h"

class ITelephonyManager {
public:
	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void handleOutgoingCall(std::string from, std::string to) = 0;
	virtual void handleAnswer(std::string connId, std::string from) = 0;
	virtual void handleReject(std::string connId, std::string cause, std::string from) = 0;
	virtual void handleDisconnect(std::string connId) = 0;
};