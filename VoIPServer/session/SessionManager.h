#pragma once

#include <map>
#include <string>
#include <vector>
#include <thread>

#include "SessionControl.h"
#include "ITelephonyManager.h"
#include "IAccountManager.h"

constexpr auto PACKET_SIZE = 1024;

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	int serverPort = 5555;
	const int MAX_CLIENTS = 10;  // 최대참여자 수
	std::map<std::string, int> clientMap;
	std::vector<std::thread> clientThread;
	std::string myIp;

	int contactNum; // For TEST

	ITelephonyManager* telephonyManager;
	IAccountManager* accountManager;

	SessionManager();

public:
	static SessionManager* getInstance();
	static void releaseInstance();

	void init();
	void release();
	void openSocket();
	void HandleClient(int clientSocket);
	std::string GetClientName(int clientSocket);
	void getMyIp();

	// interface
	void sendData(const char* message, std::string to) override;
	void sendData(int msgId, Json::Value payload, std::string to) override;
};

