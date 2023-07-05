#include <iostream>

#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"
#include "session/DatabaseManager.h"
#include "session/ContactDb.h"
#include "session/ConferenceDb.h"
#pragma comment (lib, "../json/jsoncpp_static.lib")

static void testDatabase() {
    //------------------------------------------------
    DatabaseManager* contactDb = ContactDb::getInstance();    
    
    // Add new contact data to database
    Json::Value newContact;
    newContact["cid"] = "bruno.yoo@gmail.com";
    newContact["email"] = "bruno.yoo@gmail.com";
    newContact["ipAddress"] = "123.456.789.1";
    newContact["myContactList"][0]["cid"] = "kistone@lge.com";
    newContact["myContactList"][1]["cid"] = "superman@lge.com";
    contactDb->update("bruno.yoo@gmail.com", newContact);

    // Get data from conference database
    Json::Value bruno = contactDb->get("bruno.yoo@gmail.com");
    for (int i = 0; i < bruno["myContactList"].size(); i++) {
        cout << bruno["myContactList"][i] << endl;
    }
    //------------------------------------------------
    DatabaseManager* conferenceDb = ConferenceDb::getInstance();

    // Add new conference data to database
    Json::Value conference;
    conference["rid"] = "C_00000111";
    conference["dateAndTime"] = "1234562737";
    conference["duration"] = "3600000";
    conference["participants"][0]["cid"] = "kistone@lge.com";
    conference["participants"][1]["cid"] = "superman@lge.com";
    conferenceDb->update("C_00000111", conference);
}

int main() {

    //testDatabase();

    // UI Thread
    SessionManager* sessionManager = SessionManager::getInstance();
    //sessionManager->init();
    std::thread t(&SessionManager::init, sessionManager);

    // TEST CODE
    while (true) {
        std::string message;
        getline(std::cin, message);

        if (message.empty()) {
            SessionManager::releaseInstance();
            break;
        }
    }
    t.join();

    TelephonyManager::releaseInstance();
    AccountManager::releaseInstance();
    std::cout << "Exit Main Thread" << std::endl;

    return 0;
}

