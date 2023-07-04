
#include "../../json/json.h"
using namespace std;

#pragma once
#define DB_MAX_BUFFER_SIZE		40960
#define DB_CONTACT				"contacts"
#define DB_CONFERENCE			"conferences"
#define DB_CONTACT_ITEM_ID		"cid"
#define DB_CONFERENCE_ITEM_ID	"rid"

class DatabaseManager {
private:
	//-----------------------------------------
	// Internel methods	
	Json::Value readFromFile();
	bool writeToFile(const Json::Value jsonData);
public:	
	DatabaseManager(string databaseName); // Constructor	
	//-----------------------------------------
	// Database operations
	Json::Value get(string id);
	bool remove(string id);
	bool update(string id, Json::Value data);
	bool update(string id, string key, string value);
	bool updateSub(string id, string key, Json::Value value);
	//-----------------------------------------
	// Test code for database
	void printDatabase();
	static void printJson(Json::Value jsonData);
};