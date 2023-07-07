#pragma once

#include <iostream>
#include <string>
#include <list>

#include "../../json/json.h"

class Connection {
private:
	std::string id;
	bool conference;
	std::list<std::string> participants;
	// conference only
	long startTime;
	long duration;
	std::list<std::string> conferenceList;

public:
	Connection();
	Connection(std::string id);
	Connection(std::string id, Json::Value conferenceInfo);

	std::string getId();
	bool isConference();
	void setParticipant(std::string participant);
	void removeParticipant(std::string participant);
	std::list<std::string> getParticipants();
	// conference only
	long getConferenceStartTime();
	long getDuration();
	bool isOnTime();
	std::list<std::string> getConferenceList();

	void logConnections();
};