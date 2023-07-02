#pragma once

#include "SessionControl.h"
#include "CallsManagerListener.h"
#include "AccountManagerListener.h"

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	char serverIP[20];
	int serverPort;

	CallsManagerListener* callsListener;
	AccountManagerListener* accountListener;
	
	SessionManager();

public:
	static SessionManager* getInstance();

	void setCallsListener(CallsManagerListener* listener);
	void setAccountListener(AccountManagerListener* listener);
	void init(const char* ip, int port);
	void openSocket();
	void proc_recv();

	// interface
	int sendData(const char* message) override;
};