#pragma once

class SessionControl {
public:
	virtual void sendData(const char* data, std::string to) = 0;
};