#include "DumpMediaManager.h"

DumpMediaManager* DumpMediaManager::instance = nullptr;

DumpMediaManager::DumpMediaManager() {
	sessionCallback = nullptr; // �߰�
}

DumpMediaManager* DumpMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new DumpMediaManager();
	}
	return instance;
}

// �߰�
void DumpMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback = callback;
}

// �ʿ��Ѱ�
void DumpMediaManager::anyfunc() {
	int quality = 4; // should change 3 -> 4
	sessionCallback->notifyVideoQualityChanged("rid", quality); // ȣ��
}

void DumpMediaManager::updateClientVideoQuality(Json::Value data) {}
void DumpMediaManager::startCall(Json::Value room_creat_info) {}
void DumpMediaManager::endCall(Json::Value room_remove_info) {}
void DumpMediaManager::addClient(Json::Value add_client_info) {}
void DumpMediaManager::removeClient(Json::Value remove_client_info) {}