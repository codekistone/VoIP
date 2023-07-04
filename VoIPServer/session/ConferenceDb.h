
#include "DatabaseManager.h"

#pragma once

class ConferenceDb : public DatabaseManager {
private:
	static ConferenceDb* instance;
	ConferenceDb(string dbName) : DatabaseManager(dbName) {}; // Constructor	
public:
	static ConferenceDb* getInstance();
};