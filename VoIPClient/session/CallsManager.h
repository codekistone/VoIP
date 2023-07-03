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
	void startOutgoingCall(std::string to);
	void answerCall();
	void rejectCall();
	void disconnectCall();

	// Listener
	void onIncomingCall(std::string connId, std::string from) override;
	void onSuccessfulOutgoingCall(std::string connId) override;
	void onSuccessfulIncomingCall() override;
	void onFailedOutgoingCall(std::string cause) override;
	void onRejectedIncomingCall() override;
	void onDisconnected() override;
};
