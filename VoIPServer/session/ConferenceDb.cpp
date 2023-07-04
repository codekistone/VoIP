#include "ConferenceDb.h"
#include <iostream>
using namespace std;

ConferenceDb* ConferenceDb::instance = nullptr;

ConferenceDb* ConferenceDb::getInstance()
{
	if (instance == nullptr) {
		instance = new ConferenceDb("conferences");
	}
	return instance;
}
