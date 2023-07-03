#include "Call.h"
#include "Constants.h"

Call::Call() {
	state = CallState::STATE_IDLE;
}

void Call::setCallId(std::string id) {
	callId = id;
}

std::string Call::getCallId() {
	return callId;
}

void Call::setCallState(int state) {
	this->state = state;
}

int Call::getCallState() {
	return state;
}

void Call::setContactId(std::string contactId) {
	callerContactId = contactId;
}

std::string Call::getContactId() {
	return callerContactId;
}