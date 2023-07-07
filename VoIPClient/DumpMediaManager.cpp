#include "DumpMediaManager.h"

DumpMediaManager* DumpMediaManager::instance = nullptr;

DumpMediaManager::DumpMediaManager() {
	sessionCallback = nullptr; // 추가
}

DumpMediaManager* DumpMediaManager::getInstance() {
	if (instance == nullptr) {
		instance = new DumpMediaManager();
	}
	return instance;
}

// 추가
void DumpMediaManager::setSessionCallback(ISessionMediaCallback* callback) {
	sessionCallback = callback;
}

// 필요한곳
void DumpMediaManager::anyfunc() {
	int quality = 4; // should change 3 -> 4
	sessionCallback->requestVideoQualityChange(quality); // 호출
}

void DumpMediaManager::setVideoQuality(int quality) {

};

void DumpMediaManager::startCall(Json::Value client_info) {
};
void DumpMediaManager::endCall(Json::Value client_info) {
};