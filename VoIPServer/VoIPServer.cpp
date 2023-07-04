﻿#include <iostream>

#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"

#include "session/DatabaseManager.h"
#include "session/ContactDb.h"
#include "session/ConferenceDb.h"
#pragma comment (lib, "../json/jsoncpp_static.lib")

static void testDatabase() {
    DatabaseManager* contactDb = ContactDb::getInstance();
    DatabaseManager* conferenceDb = ConferenceDb::getInstance();

    //------------------------------------------------
    // Add new contact data to databae 
    Json::Value newContact;
    newContact["cid"] = "bruno.yoo@gmail.com";
    newContact["email"] = "bruno.yoo@gmail.com";
    newContact["ipAddress"] = "123.456.789.1";
    newContact["myContactList"][0]["cid"] = "kistone@lge.com";
    newContact["myContactList"][1]["cid"] = "superman@lge.com";
    contactDb->update("bruno.yoo@gmail.com", newContact);

    //------------------------------------------------
    // Get data from contacts database
    Json::Value bruno = contactDb->get("bruno.yoo@gmail.com");
    for (int i = 0; i < bruno["myContactList"].size(); i++) {
        cout << bruno["myContactList"][i] << endl;
    }
}

int main() {

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

