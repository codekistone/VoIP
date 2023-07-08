#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

class ClientMediaManager
{
private:
	static ClientMediaManager* instance;

	ISessionMediaCallback* sessionCallback; //추가

	ClientMediaManager();
public:
	static ClientMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //추가
	void setVideoQuality(int quality);
	void anyfunc();
	void startCall(Json::Value client_info);
	void endCall(Json::Value client_info);
};
