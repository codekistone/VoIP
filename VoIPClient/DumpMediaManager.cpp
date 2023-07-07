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

void setVideoQuality(int quality) {

};

void startCall(Json::Value client_info) {
};
void endCall(Json::Value client_info) {
};