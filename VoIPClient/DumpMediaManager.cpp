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
	sessionCallback->requestVideoQualityChange(quality); // ȣ��
}

void DumpMediaManager::setVideoQuality(int quality) {

};

void DumpMediaManager::startCall(Json::Value client_info) {
};
void DumpMediaManager::endCall(Json::Value client_info) {
};