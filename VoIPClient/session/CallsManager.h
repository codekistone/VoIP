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
	void disconnectCall();

	// Listener
	void onSuccessfulOutgoingCall() override;
	void startIncomingCall() override;
	void onSuccessfulIncomingCall() override;
	void onDisconnected() override;
};
