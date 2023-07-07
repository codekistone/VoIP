#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

class DumpMediaManager
{
private:
	static DumpMediaManager* instance;

	ISessionMediaCallback* sessionCallback; //�߰�

	DumpMediaManager();
public:
	static DumpMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //�߰�
	void setVideoQuality(int quality);
	void anyfunc();
	void startCall(Json::Value client_info);
	void endCall(Json::Value client_info);
};
