#pragma once

#include "../../json/json.h"

class SessionControl {
public:
	virtual int sendData(const char* data) = 0;
	virtual int sendData(int msgId, Json::Value payload) = 0;
};