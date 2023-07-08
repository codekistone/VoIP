#pragma once

#include "../../json/json.h"

class SessionControl {
public:
	virtual void sendData(const char* data, std::string to) = 0;
	virtual void sendData(int msgId, Json::Value payload, std::string to) = 0;
	virtual std::string getMyIp() = 0;
};