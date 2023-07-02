#pragma once

#include "CallsManagerListener.h"
#include "Call.h"
#include "SessionControl.h"

class CallsManager : public CallsManagerListener {
private:
	static CallsManager* instance;

	Call* call;
	SessionControl* sessionControl;

	CallsManager();

public:
	static CallsManager* getInstance();

	void setSessionControl(SessionControl* control);
	void startOutgoingCall(const char* to);
	void answerCall();
	void rejectCall();
	void disconnectCall();

	// Listener
	void onIncomingCall(const char* from) override;
	void onSuccessfulOutgoingCall() override;
	void onSuccessfulIncomingCall() override;
	void onFailedOutgoingCall() override;
	void onRejectedIncomingCall() override;
	void onDisconnected() override;
};
