#pragma once

#include <map>

#include "SessionControl.h"
#include "ITelephonyManager.h"
#include "ISessionMediaCallback.h"
#include "Connection.h"
#include "ConferenceDb.h"
#include "ContactDb.h"
#include "../../json/json.h"

class TelephonyManager : public ITelephonyManager, public ISessionMediaCallback {
private:
	static TelephonyManager* instance;

	std::map<std::string, Connection> connectionMap;

	SessionControl* sessionControl;
	ConferenceDb* conferenceDb; // Conference Database
	ContactDb* contactDb;

	TelephonyManager();

public:
	static TelephonyManager* getInstance();
	static void releaseInstance();

	void release();
	string generateConnectionId();
	void onAnswer(Json::Value data);
	void onReject(Json::Value data);
	void postConferenceCreated(std::string connId, std::string myIp);
	int joinableConference(Json::Value data);
	void removeConference(std::string connId);
	void manageConferenceLifetime(std::string connId);
	void logConnections();

	// Listener
	void setSessionControl(SessionControl* control) override;
	void initializeConference(std::string myIp) override;
	void handleOutgoingCall(Json::Value data) override;
	void handleOutgoingCallNoUser(Json::Value data) override;
	void handleIncomingCallResponse(Json::Value data) override;
	void handleDisconnect(Json::Value data) override;
	void releaseConnection(std::string cid) override;
	void handleCreateConference(Json::Value data) override;
	void handleJoinConference(Json::Value data) override;
	void handleExitConference(Json::Value data) override;
	void handleRequestVideoQualityChange(Json::Value data) override;

	// Media Interface
	void notifyVideoQualityChanged(std::string rid, int quality) override;
};