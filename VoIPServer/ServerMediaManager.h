#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

class ServerMediaManager
{
private:
	static ServerMediaManager* instance;

	ISessionMediaCallback* sessionCallback; //추가

	ServerMediaManager();
public:
	static ServerMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //추가
	void anyfunc();
	void updateClientVideoQuality(Json::Value data); //std::string rid, std::string cid, int level
	void startCall(Json::Value room_creat_info);
	void endCall(Json::Value room_remove_info);
	void addClient(Json::Value add_client_info);
	void removeClient(Json::Value remove_client_info);

	Json::Value getMediaProperty();
};