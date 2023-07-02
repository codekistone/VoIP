#pragma once

class CallsManagerListener {
public:
	virtual void onSuccessfulOutgoingCall() = 0;
	virtual void startIncomingCall() = 0;
	virtual void onSuccessfulIncomingCall() = 0;
	virtual void onDisconnected() = 0;
};