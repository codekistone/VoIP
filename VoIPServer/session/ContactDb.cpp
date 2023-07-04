#include "ContactDb.h"
#include <iostream>
using namespace std;

ContactDb* ContactDb::instance = nullptr;

ContactDb* ContactDb::getInstance()
{
	if (instance == nullptr) {
		instance = new ContactDb(DB_CONTACT);
	}
	return instance;
}
