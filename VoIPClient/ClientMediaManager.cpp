#include "ClientMediaManager.h"

ClientMediaManager* ClientMediaManager::instance = nullptr;

ClientMediaManager::ClientMediaManager() {
	sessionCallback = nullptr; // 추가
}

ClientMediaManager* ClientMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new ClientMediaManager();
	}
	return instance;
}

// 추가
void ClientMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback = callback;
}

// 필요한곳
void ClientMediaManager::anyfunc() {
	int quality = 4; // should change 3 -> 4
	sessionCallback->requestVideoQualityChange(quality); // 호출
}

void ClientMediaManager::setVideoQuality(int quality) {}
void ClientMediaManager::startCall(Json::Value client_info) {}
void ClientMediaManager::endCall(Json::Value client_info) {}