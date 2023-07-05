
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
	string dbName;
	string dbUid;
	Json::Value readFromFile();
	bool writeToFile(const Json::Value jsonData);
public:	
	DatabaseManager(string databaseName); // Constructor	
	//-----------------------------------------
	// Database operations
	string search(string key, string word); 

	Json::Value get();
	Json::Value get(string id); 
	Json::Value get(string id, string key);  

	bool remove(string id);
	bool remove(string id, string key);
	bool remove(string id, string key, Json::Value value);

	bool update(string id, Json::Value data);
	bool update(string id, string key, Json::Value value);
	//-----------------------------------------
	// Test code for database	
	void printDatabase();
	static void printJson(Json::Value jsonData);
};