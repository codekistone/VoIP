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

void TelephonyManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

// Implement listener
void TelephonyManager::handleOutgoingCall(std::string from, std::string to) {
	std::string connId("CONNECTION_" + std::to_string(connNum++));
	Connection conn(connId);
	conn.setParticipant(from);
	conn.setParticipant(to);

	connectionMap.insert({ connId, conn });

	sessionControl->sendData(("onIncomingCall," + std::string(from) + "," + connId).c_str(), to);
}

void TelephonyManager::handleAnswer(std::string connId, std::string from) {
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
	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		sessionControl->sendData("onDisconnected", participant);
	}
	connectionMap.erase(connId);
}