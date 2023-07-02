#pragma once

class Call {
private:
	int state;
	const char* callerContactId;
	bool isConference;

public:
	Call(const char* callerContactId);
	void setCallState(int state);
	int getCallState();
	void setContactId(const char* contactId);
	const char* getContactId();
};