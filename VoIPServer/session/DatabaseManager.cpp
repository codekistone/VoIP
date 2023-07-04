#include "DatabaseManager.h"
#include <iostream>

string dbName;
string dbItemId;
static char readBuffer[DB_MAX_BUFFER_SIZE] = { 0, };

DatabaseManager::DatabaseManager(string databaseName)
{	
	dbName = databaseName;
	dbItemId = DB_CONTACT_ITEM_ID;
	if (dbName == DB_CONFERENCE) {
		dbItemId = DB_CONFERENCE_ITEM_ID;
	}
	if (readFromFile() == false) {
		// Database file does not exist, create default database file.
		Json::Value defaultData;
		defaultData[dbName] = Json::arrayValue;
		writeToFile(defaultData);		
	}
}

void DatabaseManager::printDatabase()
{
	Json::Value root = readFromFile();
	printJson(root);
}

void DatabaseManager::printJson(Json::Value jsonData)
{
	if (jsonData != NULL) {
		Json::StreamWriterBuilder builder;
		const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
		writer->write(jsonData, &std::cout);
		std::cout << endl;
	} 
}

Json::Value DatabaseManager::readFromFile()
{
	string fileName = dbName + ".db";
	// Read data from file
	FILE* fp = nullptr;
	fopen_s(&fp, &*fileName.begin(), "rb");
	if (fp == nullptr) {
		return false;
	}
	size_t fileSize = fread(readBuffer, 1, DB_MAX_BUFFER_SIZE, fp);
	fclose(fp);
	// Parse readbuffer as JSON
	std::string config_doc = readBuffer;
	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse(config_doc, root);
	if (parsingSuccessful == false) {
		return false;
	}	
	return root;
}

bool DatabaseManager::writeToFile(const Json::Value jsonData)
{
	string fileName = dbName + ".db";
	Json::StyledWriter writer;
	std::string outputConfig = writer.write(jsonData);
	// Write data to FILE
	FILE* fp = nullptr;
	fopen_s(&fp, &*fileName.begin(), "wb");
	if (fp == nullptr) {
		return false;
	}
	size_t fileSize = fwrite(outputConfig.c_str(), 1, outputConfig.length(), fp);
	fclose(fp);
	return true;
}

Json::Value DatabaseManager::get(string id)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i][dbItemId] == id) {
				cout << "get() : " << datas[i] << endl;
				return datas[i];
			}
		}
	}
	catch (std::exception ex) {
		cout << "get()/Exception : " << ex.what() << endl;
	}
	return NULL;
}

bool DatabaseManager::remove(string id)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1;
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i][dbItemId] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			cout << "remove()/Not found/Id[" << id << "]" << endl;
			return false;
		}
		cout << "remove()/id[" << id << "]/data:" << datas[index] << endl;
		datas.removeIndex(index, NULL);
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		cout << "remove()/Exception : " << ex.what() << endl;
	}
	return false;
}

bool DatabaseManager::update(string id, Json::Value data)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int searchIdx = -1;
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i][dbItemId] == id) {
				searchIdx = i;
				break;
			}
		}
		data[dbItemId] = id;
		if (searchIdx == -1) {
			cout << "update()/New/Id[" << id << "]/data:" << data << endl;
			datas.append(data);
		}
		else {
			cout << "update()/Update/Id[" << id << "]/data:" << data << endl;
			datas[searchIdx] = data;
		}
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		cout << "update()/Exception : " << ex.what() << endl;
	}
	return false;
}

bool DatabaseManager::update(string id, string key, string value)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1, subindex = -1;
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i][dbItemId] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			cout << "update()/Not found/Id[" << id << "]" << endl;
			return false; // Failed to find contact to update
		}
		if (datas[index][key].empty()) {
			cout << "update()/Add/Id[" << id << "]/Key[" << key << "]/Value[" + value << "]" << endl;
		}
		else {
			cout << "update()/Update/Id[" << id << "]/Key[" << key << "]/Value[" + value << "]" << endl;
		}
		datas[index][key] = value;
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		cout << "update()/Exception : " << ex.what() << endl;
	}
	return false;
}

bool DatabaseManager::updateSub(string id, string key, Json::Value value)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1, subindex = -1;
		for (int i = 0; i < datas.size(); i++) {
			if (datas[i][dbItemId] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			cout << "updateSub()/Not found/Id[" << id << "]" << endl;
			return false; // Failed to find contact to update
		}
		int subIndex = -1;
		for (int j = 0; j < datas[index][key].size(); j++) {
			if (datas[index][key][j] == value) {
				subIndex = j;
				break;
			}
		}
		if (subIndex == -1) {
			cout << "updateSub()/Append/Id[" << id << "]/Key[" << key << "]/Value[" << value << "]" << endl;
			datas[index][key].append(value);
		}
		else {
			cout << "updateSub()/Update/Id[" << id << "]/Key[" << key << "]/Value[" << value << "]" << endl;
			datas[index][key][subIndex] = value;
		}
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		cout << "updateSub()/Exception : " << ex.what() << endl;
	}
	return false;
}
