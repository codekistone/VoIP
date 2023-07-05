#include <iostream>

#include "TelephonyManager.h"

TelephonyManager* TelephonyManager::instance = nullptr;

int connNum = 0;

TelephonyManager::TelephonyManager() {
	sessionControl = nullptr;
}

TelephonyManager* TelephonyManager::getInstance() {
	if (instance == nullptr) {
		instance = new TelephonyManager();
	}
	return instance;
}

void TelephonyManager::releaseInstance() {
	if (instance != nullptr) {
		instance->setSessionControl(nullptr);
		delete instance;
		instance = nullptr;
		std::cout << "TelephonyManager::releaseInstance" << std::endl;
	}
}

// Implement listener
void TelephonyManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void TelephonyManager::handleOutgoingCall(std::string from, std::string to) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string connId("CONNECTION_" + std::to_string(connNum++));
	Connection conn(connId);
	conn.setParticipant(from);
	conn.setParticipant(to);

	connectionMap.insert({ connId, conn });

	sessionControl->sendData(("onIncomingCall," + std::string(from) + "," + connId).c_str(), to);
}

void TelephonyManager::handleAnswer(std::string connId, std::string from) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			sessionControl->sendData("onSuccessfulIncomingCall", from);
			continue;
		}
		sessionControl->sendData(("onSuccessfulOutgoingCall," + connId).c_str(), participant);
	}
}

void TelephonyManager::handleReject(std::string connId, std::string cause, std::string from) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			if (cause == "rejected") {
				sessionControl->sendData("onRejectedIncomingCall", from);
			}
			continue;
		}
		sessionControl->sendData(("onFailedOutgoingCall," + cause).c_str(), participant);
	}
	connectionMap.erase(connId);
}

void TelephonyManager::handleDisconnect(std::string connId) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		sessionControl->sendData("onDisconnected", participant);
	}
	connectionMap.erase(connId);
}