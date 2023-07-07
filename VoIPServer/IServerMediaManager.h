#pragma once

#include <vector>
#include "../json/json.h"

struct IServerMediaManager {
public:
	virtual void registerNotifyTargetVideoQualityCallback(void (*notifyTargetVideoQuality)(int, int)) = 0;
	virtual void updateClientVideoQuality(Json::Value data) = 0; // rid, cid, quality
	virtual void startCall(Json::Value room_creat_info) = 0;
	virtual void endCall(Json::Value room_remove_info) = 0;
	virtual void addClient(Json::Value add_client_info) = 0;
	virtual void removeClient(Json::Value remove_client_info) = 0;
};