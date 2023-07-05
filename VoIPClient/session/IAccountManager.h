#pragma once

#include <iostream>

#include "SessionControl.h"

class IAccountManager {
public:

	virtual void setSessionControl(SessionControl* control) = 0;
	virtual void onLoginSuccess(std::string contactId) = 0;
};