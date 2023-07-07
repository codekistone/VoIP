#pragma once

#include "ICallsManager.h"
#include "Call.h"
#include "SessionControl.h"
#include "ISessionMediaCallback.h"
#include "../../json/json.h"

class CallsManager : public ICallsManager, public ISessionMediaCallback {
private:
	static CallsManager* instance;

	Call* call;
	SessionControl* sessionControl;

	CallsManager();

public:
	static CallsManager* getInstance();
	static void releaseInstance();

	void startOutgoingCall(std::string to);
	void onSuccessfulOutgoingCall(Json::Value data);
	void onFailedOutgoingCall(Json::Value data);
	void onSuccessfulIncomingCall(Json::Value data);
	void onRejectedIncomingCall();
	void answerCall();
	void rejectCall();
	void disconnectCall();
	void joinConference(std::string rid);
	void onSuccessfulJoinConference(Json::Value data);
	void onFailedJoinConference(Json::Value data);
	void exitConference(std::string rid);

	// Session Interface
	void setSessionControl(SessionControl* control) override;
	void onOutgoingCallResult(Json::Value data) override;
	void onIncomingCall(Json::Value data) override;
	void onIncomingCallResult(Json::Value data) override;
	void onDisconnected(Json::Value data) override;
	void onJoinConferenceResult(Json::Value data) override;
	void onExitConference(Json::Value data) override;
	void onVideoQualityChanged(Json::Value data) override;

	// Media Interface
	void requestVideoQualityChange(int quality) override;
};
