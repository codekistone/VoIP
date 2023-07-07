#pragma once
#include "../json/json.h"

struct IClientMediaManager {
	virtual void registerRequestVideoQualityCallback(void (*notifyVideoQuality)(int)) = 0;
	virtual void setVideoQuality(int video_quality_index) = 0;
	virtual void startCall(Json::Value client_info) = 0;
	virtual void endCall(Json::Value client_info) = 0;
};