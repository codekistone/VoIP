#pragma once

#include <string>

class Call {
private:
	int state;
	std::string callId;
	std::string callerContactId;
	bool isConference;

public:
	Call();
	void setCallId(std::string id);
	std::string getCallId();
	void setCallState(int state);
	int getCallState();
	void setContactId(std::string contactId);
	std::string getContactId();
};