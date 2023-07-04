
#include "../../json/json.h"
using namespace std;

#pragma once

#define DB_ITEM_ID				"id"
#define DB_MAX_BUFFER_SIZE		40960

class DatabaseManager {
private:
	string DB_NAME;
	//-----------------------------------------
	// Internel methods	
	Json::Value readFromFile();
	bool writeToFile(const Json::Value jsonData);
public:	
	DatabaseManager(string dbName); // Constructor	
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