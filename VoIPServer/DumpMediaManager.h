#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

class DumpMediaManager
{
private:
	static DumpMediaManager* instance;

	ISessionMediaCallback* sessionCallback; //추가

	DumpMediaManager();
public:
	static DumpMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //추가
	void anyfunc();
	void updateClientVideoQuality(Json::Value data); //std::string rid, std::string cid, int level
	void startCall(Json::Value room_creat_info);
	void endCall(Json::Value room_remove_info);
	void addClient(Json::Value add_client_info);
	void removeClient(Json::Value remove_client_info);
};