#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

namespace media {

class ClientMediaManager
{
private:
	static ClientMediaManager* instance;

	ISessionMediaCallback* sessionCallback_; //�߰�

	ClientMediaManager();
public:
	static ClientMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //�߰�
	void setVideoQuality(int quality);
	void anyfunc();
	void startCall(Json::Value client_info);
	void endCall(Json::Value client_info);
};

} // namespace media