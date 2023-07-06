#pragma once

#include <map>

#include "SessionControl.h"
#include "ITelephonyManager.h"
#include "Connection.h"
#include "../../json/json.h"

class TelephonyManager : public ITelephonyManager {
private:
	static TelephonyManager* instance;

	std::map<std::string, Connection> connectionMap;

	SessionControl* sessionControl;

	TelephonyManager();

public:
	static TelephonyManager* getInstance();
	static void releaseInstance();

	void handleAnswer(Json::Value data);
	void handleReject(Json::Value data);
	bool joinableConference(Json::Value data);

	// Listener
	void setSessionControl(SessionControl* control) override;
	void handleOutgoingCall(Json::Value data) override;
	void handleOutgoingCallNoUser(Json::Value data) override;
	void handleIncomingCallResponse(Json::Value data) override;
	void handleDisconnect(Json::Value data) override;
	void releaseConnection(std::string cid) override;
	void handleCreateConference(Json::Value data) override;
	void handleJoinConference(Json::Value data) override;
	void handleExitConference(Json::Value data) override;
};