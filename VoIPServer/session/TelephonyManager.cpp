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

std::string makeMessage(int msgId, Json::Value payload) {
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;

	std::string jsonString = fastWriter.write(root);
	return jsonString;
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

	sessionControl->sendData(makeMessage(302, payload).c_str(), to);
}

void TelephonyManager::handleOutgoingCallNoUser(Json::Value data) {
	std::string from = data["from"].asString();
	Json::Value payload;
	payload["rid"] = "UNKNOWN";
	payload["result"] = 2;
	payload["cause"] = 3;
	sessionControl->sendData(makeMessage(301, payload).c_str(), from);
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
			sessionControl->sendData(makeMessage(303, payload).c_str(), from);
			continue;
		}
		sessionControl->sendData(makeMessage(301, payload).c_str(), participant);
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
				sessionControl->sendData(makeMessage(303, payload).c_str(), from);
			}
			continue;
		}
		sessionControl->sendData(makeMessage(301, payload).c_str(), participant);
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
		sessionControl->sendData(makeMessage(305, payload).c_str(), participant);
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