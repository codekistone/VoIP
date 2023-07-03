#pragma once

#include <iostream>
#include <string>
#include <list>

class Connection {
private:
	std::string id;
	bool isConference;
	std::list<std::string> participants;

public:
	Connection();
	Connection(std::string id);

	std::string getId();
	void setParticipant(std::string participant);
	std::list<std::string> getParticipants();
};