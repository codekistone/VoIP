#include "Connection.h"

Connection::Connection() {};
Connection::Connection(std::string connId) {
	id = connId;
};

std::string Connection::getId() {
	return id;
}

void Connection::setParticipant(std::string participant) {
	participants.push_back(participant);
}

std::list<std::string> Connection::getParticipants() {
	return participants;
}