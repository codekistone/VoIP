#pragma once

class CallsManagerListener {
public:
	virtual void onIncomingCall(const char* from) = 0;
	virtual void onSuccessfulOutgoingCall() = 0;
	virtual void onSuccessfulIncomingCall() = 0;
	virtual void onFailedOutgoingCall() = 0;
	virtual void onRejectedIncomingCall() = 0;
	virtual void onDisconnected() = 0;
};