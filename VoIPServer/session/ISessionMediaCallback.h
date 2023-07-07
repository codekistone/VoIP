#pragma once

#include <string>

class ISessionMediaCallback {
public:
	virtual void notifyVideoQualityChanged(std::string rid, int quality) = 0;
};