#pragma once

#include <map>
#include <string>
#include <vector>
#include <thread>

#include "SessionControl.h"
#include "ITelephonyManager.h"
#include "IAccountManager.h"
#include "../../json/json.h"

constexpr auto PACKET_SIZE = 1024;

class SessionManager : public SessionControl {
private:
	static SessionManager* instance;

	int serverPort = 5555;
	const int MAX_CLIENTS = 10;  // �ִ� Ŭ���̾�Ʈ ��
	std::map<std::string, int> clientMap;
	std::vector<std::thread> clientThread;

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
	std::vector<std::string> split(const std::string& str, char delimiter);

	// interface
	void sendData(const char* message, std::string to) override;
};
