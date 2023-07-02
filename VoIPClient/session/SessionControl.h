#pragma once

class SessionControl {
public:
	virtual int sendData(const char* data) = 0;
};