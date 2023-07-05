#pragma once

#include "ICallsManager.h"
#include "Call.h"
#include "SessionControl.h"
#include "../../json/json.h"

class CallsManager : public ICallsManager {
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
	void onSuccessfulIncomingCall();
	void onRejectedIncomingCall();
	void answerCall();
	void rejectCall();
	void disconnectCall();

	// Listener
	void setSessionControl(SessionControl* control) override;
	void onOutgoingCallResult(Json::Value data) override;
	void onIncomingCall(Json::Value data) override;
	void onIncomingCallResult(Json::Value data) override;
	void onDisconnected(Json::Value data) override;
};
