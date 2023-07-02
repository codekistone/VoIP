#include "Call.h"
#include "Constants.h"

Call::Call(const char* contactId) {
	callerContactId = contactId;
	state = CallState::STATE_IDLE;
}

void Call::setCallState(int state) {
	this->state = state;
}

int Call::getCallState() {
	return state;
}

void Call::setContactId(const char* contactId) {
	callerContactId = contactId;
}

const char* Call::getContactId() {
	return callerContactId;
}