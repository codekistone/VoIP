#include "ClientMediaManager.h"

ClientMediaManager* ClientMediaManager::instance = nullptr;

ClientMediaManager::ClientMediaManager() {
	sessionCallback = nullptr; // �߰�
}

ClientMediaManager* ClientMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new ClientMediaManager();
	}
	return instance;
}

// �߰�
void ClientMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback = callback;
}

// �ʿ��Ѱ�
void ClientMediaManager::anyfunc() {
	int quality = 4; // should change 3 -> 4
	sessionCallback->requestVideoQualityChange(quality); // ȣ��
}

void ClientMediaManager::setVideoQuality(int quality) {}
void ClientMediaManager::startCall(Json::Value client_info) {}
void ClientMediaManager::endCall(Json::Value client_info) {}