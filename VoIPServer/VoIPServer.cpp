#include <iostream>

#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"

#include "session/DatabaseManager.h"
#include "session/ContactDb.h"
#include "session/ConferenceDb.h"
#pragma comment (lib, "../json/jsoncpp_static.lib")

int main() {

	// [START] TBD : Need to move initialization code into SessionManager or AccountManager
	DatabaseManager* contactDb = ContactDb::getInstance();
	DatabaseManager* conferenceDb = ConferenceDb::getInstance();
    contactDb->printDatabase();
    conferenceDb->printDatabase();
    // [END] TBD

    SessionManager* sessionManager = SessionManager::getInstance();
    AccountManager* accountManager = AccountManager::getInstance();
    TelephonyManager* telephonyManager = TelephonyManager::getInstance();
    sessionManager->setAccountListener(accountManager);
    sessionManager->setTelephonyListener(telephonyManager);

    accountManager->setSessionControl(sessionManager);
    telephonyManager->setSessionControl(sessionManager);

    std::thread t(&SessionManager::init, sessionManager);
    t.join();

    return 0;
}

