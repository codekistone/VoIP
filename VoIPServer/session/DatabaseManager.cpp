#include "DatabaseManager.h"
#include <iostream>


static char readBuffer[DB_MAX_BUFFER_SIZE] = { 0, };

DatabaseManager::DatabaseManager(string databaseName)
{	
	dbName = databaseName;
	dbUid = DB_CONTACT_ITEM_ID;
	if (dbName == DB_CONFERENCE) {
		dbUid = DB_CONFERENCE_ITEM_ID;
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

string DatabaseManager::search(string key, string word) 
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][key].isArray()) {
				for (int j = 0; j < datas[i][key].size(); j++) {
					if ( datas[i][key][j].asString().compare(word) == 0 ){
						#ifdef DEBUG
						cout << "search()/OK/Key[" + key + "]/Word[" + word + "]/Id:" << datas[i][dbUid].asString() << endl;
						#endif
						return datas[i][dbUid].asString();
					}
				}
			}
			else {
				string source = datas[i][key].asString();
				if ( source.compare(word) == 0 ) {
					#ifdef DEBUG
					cout << "search()/OK/Key[" + key + "]/Word[" + word + "]/Id:" << datas[i][dbUid].asString() << endl;
					#endif
					return datas[i][dbUid].asString();
				}
			}
		}
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "search()/Exception/Key[" + key + "]/Word[" + word + "]:" << ex.what() << endl;
		#endif
		return "";
	}
	return "";
}

Json::Value DatabaseManager::get()
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		#ifdef DEBUG
		cout << "get()/OK[" << datas << "]" << endl;
		#endif
		return datas;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "get()/Exception : " << ex.what() << endl;
		#endif
		return NULL;
	}
	#ifdef DEBUG
	cout << "get()/Not found" << endl;
	#endif
	return NULL;
}

Json::Value DatabaseManager::get(string id)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				#ifdef DEBUG
				cout << "get()/OK/id[" << id << "]:" << datas[i] << endl;
				#endif
				return datas[i];
			}
		}
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "get()/Exception/id[" << id << "]:" << ex.what() << endl;
		#endif
		return NULL;
	}
	#ifdef DEBUG
	cout << "get()/Not found/id[" << id << "]:" << endl;
	#endif
	return NULL;
}

Json::Value DatabaseManager::get(string id, string key)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				#ifdef DEBUG
				cout << "get()/OK/id[" << id << "]/key[" + key + "] : " << datas[i][key] << endl;
				#endif
				return datas[i][key];
			}
		}
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "get()/Exception : " << ex.what() << endl;
		#endif
		return NULL;
	}
	#ifdef DEBUG
	cout << "get()/Not found/id[" << id << "]" << endl;
	#endif
	return NULL;
}

bool DatabaseManager::remove(string id)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1;
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			#ifdef DEBUG
			cout << "remove()/Not found/id[" << id << "]" << endl;
			#endif
			return false;
		}
		#ifdef DEBUG
		cout << "remove()/id[" << id << "]:" << datas[index] << endl;
		#endif
		datas.removeIndex(index, NULL);
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "remove()/Exception:" << ex.what() << endl;
		#endif
		return false;
	}
	return false;
}

bool DatabaseManager::remove(string id, string key)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1;
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			#ifdef DEBUG
			cout << "remove()/Not found/Id[" << id << "]" << endl;
			#endif
			return false;
		}
		if (datas[index][key].isArray()) {
			datas[index][key].clear();
		}
		else {
			datas[index].removeMember(key);
		}		
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "remove()/Exception : " << ex.what() << endl;
		#endif
	}
	return false;
}

bool DatabaseManager::remove(string id, string key, Json::Value value)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1;
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			#ifdef DEBUG
			cout << "remove()/Not found/Id[" << id << "]/key[" << key << "]" << endl;
			#endif
			return false;
		}
		if (datas[index][key].isArray()) {
			int removeIndex = -1;
			for (int j = 0; j < (int)datas[index][key].size(); j++) {
				if (datas[index][key][j] == value ) {
					removeIndex = j;
					break;
				}
			}
			if (removeIndex == -1) {
				#ifdef DEBUG
				cout << "remove()/Not found/Id[" << id << "]/key[" << key << "/Value[" << value << "]" << endl;
				#endif
				return false;
			} 			
			datas[index][key].removeIndex(removeIndex, NULL);
		} else {
			if (datas[index][key] == value) {
				datas[index].removeMember(key);
			}
			else {
				#ifdef DEBUG
				cout << "remove()/Not found/Id[" << id << "]/key[" << key << "/Value[" << value << "]" << endl;
				#endif
				return false;
			}
		}
		#ifdef DEBUG
		cout << "remove()/OK/Id[" << id << "]/Value[" << value << "]" << endl;
		#endif
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "remove()/Exception : " << ex.what() << endl;
		#endif
	}
	return false;
}

bool DatabaseManager::update(string id, Json::Value data)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int searchIdx = -1;
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				searchIdx = i;
				break;
			}
		}
		data[dbUid] = id;
		if (searchIdx == -1) {
			#ifdef DEBUG
			cout << "update()/New/Id[" << id << "]/data:" << data << endl;
			#endif
			datas.append(data);
		}
		else {
			#ifdef DEBUG
			cout << "update()/Update/Id[" << id << "]/data:" << data << endl;
			#endif
			datas[searchIdx] = data;
		}
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "update()/Exception : " << ex.what() << endl;
		#endif
	}
	return false;
}

bool DatabaseManager::update(string id, string key, Json::Value value)
{
	try {
		Json::Value database = readFromFile();
		Json::Value datas = database[dbName];
		int index = -1, subindex = -1;
		for (int i = 0; i < (int)datas.size(); i++) {
			if (datas[i][dbUid] == id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			#ifdef DEBUG
			cout << "update()/Not found/Id[" << id << "]" << endl;
			#endif
			return false; // Failed to find contact to update
		}		
		if (value.isArray()) {
			for (int i = 0; i < value.size(); i++) {
				update(id, key, value[i]);
			}
			return true;
		}
		bool keyIsForArray = false;
		if (key.compare("myContactList") == 0 || key.compare("participants") == 0) {
			keyIsForArray = true;
		}
		if (keyIsForArray && !value.isArray()) {
			for (int j = 0; j < (int)datas[index][key].size(); j++) {
				if (datas[index][key][j] == value) {
					return false; // Same sub data exists
				}
			}
			datas[index][key].append(value);
		} else {
			datas[index][key] = value;
			#ifdef DEBUG
			cout << "update()/OK/Id[" << id << "]/Key[" << key << "]/Value[" << value << "]" << endl;
			#endif
		}
		database[dbName] = datas;
		writeToFile(database);
		return true;
	}
	catch (std::exception ex) {
		#ifdef DEBUG
		cout << "update()/Exception : " << ex.what() << endl;
		#endif
	}
	return false;
}

