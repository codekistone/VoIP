#pragma once

class ISessionMediaCallback {
public:
	virtual void requestVideoQualityChange(int quality) = 0;
};