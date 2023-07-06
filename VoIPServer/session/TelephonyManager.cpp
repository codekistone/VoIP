#include <iostream>

#include "TelephonyManager.h"

TelephonyManager* TelephonyManager::instance = nullptr;
Json::FastWriter fastWriter;
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

void TelephonyManager::handleOutgoingCall(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string from = data["from"].asString();
	std::string to = data["to"].asString();

	std::string connId("CONNECTION_" + std::to_string(connNum++));
	Connection conn(connId);
	std::cout << "setParticipant: " << from << ", " << to << std::endl;
	conn.setParticipant(from);
	conn.setParticipant(to);
	connectionMap.insert({ connId, conn });

	Json::Value payload;
	payload["rid"] = connId;
	payload["cid"] = from;

	sessionControl->sendData(302, payload, to);
}

void TelephonyManager::handleOutgoingCallNoUser(Json::Value data) {
	std::string from = data["from"].asString();
	Json::Value payload;
	payload["rid"] = "UNKNOWN";
	payload["result"] = 2;
	payload["cause"] = 3;
	sessionControl->sendData(301, payload, from);
}

void TelephonyManager::handleIncomingCallResponse(Json::Value data) {
	int result = data["result"].asInt();
	if (result == 1) { // 1:ANSWER
		handleAnswer(data);
	}
	else if (result == 2) { // 2:REJECT
		handleReject(data);
	}
}

void TelephonyManager::handleAnswer(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["from"].asString();

	Json::Value payload;
	payload["rid"] = connId;
	payload["result"] = 1;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			sessionControl->sendData(303, payload, from);
			continue;
		}
		sessionControl->sendData(301, payload, participant);
	}
}

void TelephonyManager::handleReject(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["from"].asString();
	int cause = data["cause"].asInt();

	Json::Value payload;
	payload["rid"] = connId;
	payload["result"] = 2;
	payload["cause"] = cause;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			if (cause == 1) { /* reject */
				sessionControl->sendData(303, payload, from);
			}
			continue;
		}
		sessionControl->sendData(301, payload, participant);
	}
	connectionMap.erase(connId);
}

void TelephonyManager::handleDisconnect(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string connId(data["rid"].asString());

	Json::Value payload;
	payload["rid"] = connId;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		sessionControl->sendData(305, payload, participant);
	}
	connectionMap.erase(connId);
}

void TelephonyManager::releaseConnection(std::string cid) {
	std::string connId;
	for (const auto& iter : connectionMap) {
		Connection conn = iter.second;
		std::list<std::string> participants = conn.getParticipants();
		if (std::count(participants.begin(), participants.end(), cid) > 0) {
			connId = iter.first;
			break;
		}
	}
	if (!connId.empty()) {
		Json::Value payload;
		payload["rid"] = connId;
		handleDisconnect(payload);
	}
}