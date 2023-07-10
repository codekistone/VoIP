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
	IUiController* uiControl;

	CallsManager();

public:
	static CallsManager* getInstance();
	static void releaseInstance();

	Call* getCall();
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
	void exitConference();

	// Session Interface
	void setSessionControl(SessionControl* control) override;
	void setUiControl(IUiController* control) override;
	void releaseCall() override;
	void onOutgoingCall(Json::Value data) override;
	void onOutgoingCallResult(Json::Value data) override;
	void onIncomingCall(Json::Value data) override;
	void onIncomingCallResult(Json::Value data) override;
	void onDisconnected() override;
	void onJoinConferenceResult(Json::Value data) override;
	void onExitConference(Json::Value data) override;
	void onVideoQualityChanged(Json::Value data) override;

	// Media Interface
	void requestVideoQualityChange(int quality) override;
};
