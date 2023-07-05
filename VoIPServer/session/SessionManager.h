#pragma once

#include <map>
#include <string>
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "SessionControl.h"
#include "TelephonyManagerListener.h"
#include "AccountManagerListener.h"
#include "../../json/json.h"

#define PACKET_SIZE 1024

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	int serverPort = 5555;
	const int MAX_CLIENTS = 10;  // 최대 클라이언트 수
	std::map<std::string, int> clientMap;
	std::vector<std::thread> clientThread;

	int contactNum; // For TEST

	TelephonyManagerListener* telephonyListener;
	AccountManagerListener* accountListener;

	SessionManager();

public:
	static SessionManager* getInstance();

	void init();
	void openSocket();
	void HandleClient(int clientSocket);
	std::string GetClientName(int clientSocket);
	void setTelephonyListener(TelephonyManagerListener* listener);
	void setAccountListener(AccountManagerListener* listener);
	std::vector<std::string> split(const std::string& str, char delimiter);

	// interface
	void sendData(const char* message, std::string to) override;
};

