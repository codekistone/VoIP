#include "ServerMediaManager.h"

ServerMediaManager* ServerMediaManager::instance = nullptr;

ServerMediaManager::ServerMediaManager() {
	sessionCallback = nullptr; // �߰�
}

ServerMediaManager* ServerMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new ServerMediaManager();
	}
	return instance;
}

// �߰�
void ServerMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback = callback;
}

// �ʿ��Ѱ�
void ServerMediaManager::anyfunc() {
	int quality = 4; // should change 3 -> 4
	sessionCallback->notifyVideoQualityChanged("rid", quality); // ȣ��
}

void ServerMediaManager::updateClientVideoQuality(Json::Value data) {}
void ServerMediaManager::startCall(Json::Value room_creat_info) {}
void ServerMediaManager::endCall(Json::Value room_remove_info) {}
void ServerMediaManager::addClient(Json::Value add_client_info) {}
void ServerMediaManager::removeClient(Json::Value remove_client_info) {}
Json::Value ServerMediaManager::getMediaProperty() {
	Json::Value payload;
	payload["videoCodec"] = "codec";
	payload["audioCodec"] = "codec";
	payload["encryption_alg"] = "alg";
	payload["encryption_key"] = "key";
	return payload;
}