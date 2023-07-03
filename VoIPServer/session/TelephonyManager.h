#pragma once

#include <map>

#include "SessionControl.h"
#include "TelephonyManagerListener.h"
#include "Connection.h"

class TelephonyManager : public TelephonyManagerListener {
private:
	static TelephonyManager* instance;

	std::map<std::string, Connection> connectionMap;

	SessionControl* sessionControl;

	TelephonyManager();

public:
	static TelephonyManager* getInstance();

	void setSessionControl(SessionControl* control);

	// Listener
	void handleOutgoingCall(std::string from, std::string to) override;
	void handleAnswer(std::string connId, std::string from) override;
	void handleReject(std::string connId, std::string cause, std::string from) override;
	void handleDisconnect(std::string connId) override;
};