#include <iostream>

#include "session/SessionManager.h"
#include "session/TelephonyManager.h"
#include "session/AccountManager.h"

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