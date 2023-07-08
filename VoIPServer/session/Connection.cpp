#include "Connection.h"
#include <iostream>
#include <chrono>

Connection::Connection() {};
Connection::Connection(std::string connId) {
	id = connId;
	startTime = 0;
	duration = 0;
};

Connection::Connection(std::string connId, Json::Value conferenceInfo) {
	id = connId;
	conference = true;
	startTime = conferenceInfo["dateAndTime"].asInt64();
	duration = conferenceInfo["duration"].asInt64();
	for (const auto& iter : conferenceInfo["participants"]) {
		conferenceList.push_back(iter.asString());
	}
};

std::string Connection::getId() {
	return id;
}

bool Connection::isConference() {
	return conference;
}

void Connection::setParticipant(std::string participant) {
	participants.push_back(participant);
}

void Connection::removeParticipant(std::string participant) {
	participants.remove(participant);
}

std::list<std::string> Connection::getParticipants() {
	return participants;
}

long Connection::getConferenceStartTime() {
	return startTime;
}

long Connection::getDuration() {
	return duration;
}

bool Connection::isOnTime() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::duration epoch = now.time_since_epoch();
	long currentTime = std::chrono::duration_cast<std::chrono::seconds>(epoch).count();
	return currentTime >= startTime && currentTime < (startTime + duration);
}

std::list<std::string> Connection::getConferenceList() {
	return conferenceList;
}

