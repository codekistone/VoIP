#include <iostream>

#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"
#include "session/DatabaseManager.h"
#include "session/ContactDb.h"
#include "session/ConferenceDb.h"

static void testDatabase() {
    //------------------------------------------------
    // Database samples
    DatabaseManager* contactDb = ContactDb::getInstance();    
    DatabaseManager* conferenceDb = ConferenceDb::getInstance();
    
    Json::Value newContact;
    newContact["cid"] = "bruno.yoo@gmail.com";
    newContact["email"] = "bruno.yoo@gmail.com";
    newContact["name"] = "Bruno Yoo";
    newContact["ipAddress"] = "123.456.789.1";
    newContact["myContactList"][0] = "kistone@lge.com";
    newContact["myContactList"][1] = "superman@lge.com";
    contactDb->update("bruno.yoo@gmail.com", newContact);

    contactDb->update("john.mayer@gmail.com", "password", "Hello");
    contactDb->update("john.mayer@gmail.com", "myContactList", "bruno.yoo@gmail.com"); // if key is array append data to array
    contactDb->remove("john.mayer@gmail.com", "ipAddress");
    contactDb->update("john.mayer@gmail.com", "ipAddress", "1.2.3.4");
    contactDb->remove("john.mayer@gmail.com", "ipAddress", "1.2.3.4");
    contactDb->remove("john.mayer@gmail.com", "ipAddress", "10.20.30.40");
    contactDb->remove("john.mayer@gmail.com", "myContactList", "hello.there@gmail.com"); // if key is array remove item from array
    contactDb->remove("john.mayer@gmail.com", "myContactList"); //  Delete all array data
    contactDb->update("john.mayer@gmail.com", "myContactList", "abc.def@gmail.com"); // Append to array 

    Json::Value newConference;
    newConference["rid"] = "C_00000001";
    newConference["dateAndTime"] = 1688454555;
    newConference["duration"] = 3600000;
    newConference["participants"][0] = "kistone@lge.com";
    newConference["participants"][1] = "superman@lge.com";
    conferenceDb->update("C_00000001", newConference);
    
    // Get data from conference database    
    cout << "0:" << contactDb->get() << endl;
    cout << "1:" << contactDb->get("john.mayer@gmail.com") << endl;
    cout << "2:" << contactDb->get("john.mayer@gmail.com", "ipAddress") << endl;
    cout << "3:" << contactDb->get("john.mayer@gmail.com", "myContactList") << endl;
    cout << "4:" << contactDb->search("email", "john") << endl; // Return CID for search result
    cout << "5:" << contactDb->search("ipAddress", "100.2") << endl; // Return CID for search result
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

