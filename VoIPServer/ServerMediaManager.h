#pragma once

#include "session/ISessionMediaCallback.h"
#include "../json/json.h"

namespace media {
class ServerMediaManager
{
private:
	static ServerMediaManager* instance;

	ISessionMediaCallback* sessionCallback_;

	ServerMediaManager();
public:
	static ServerMediaManager* getInstance();
	void setSessionCallback(ISessionMediaCallback* callback); //Ãß°¡
	void anyfunc();
	void updateClientVideoQuality(Json::Value data); //std::string rid, std::string cid, int level
	void startCall(Json::Value room_creat_info);
	void endCall(Json::Value room_remove_info);
	void addClient(Json::Value add_client_info);
	void removeClient(Json::Value remove_client_info);

	Json::Value getMediaProperty();
};

} // namespace media