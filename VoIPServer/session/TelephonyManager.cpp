#include <iostream>
#include <ctime>

#include "TelephonyManager.h"

TelephonyManager* TelephonyManager::instance = nullptr;
Json::FastWriter fastWriter;
int connNum = 0;

TelephonyManager::TelephonyManager() {
	sessionControl = nullptr;
	conferenceDb = ConferenceDb::getInstance();
}

TelephonyManager* TelephonyManager::getInstance() {
	if (instance == nullptr) {
		instance = new TelephonyManager();
	}
	return instance;
}

void TelephonyManager::releaseInstance() {
	if (instance != nullptr) {
		instance->release();
		delete instance;
		instance = nullptr;
		std::cout << "TelephonyManager::releaseInstance" << std::endl;
	}
}

void TelephonyManager::release() {
	sessionControl = nullptr;
	conferenceDb = nullptr;
}

void TelephonyManager::onAnswer(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["from"].asString();

	Json::Value payload;
	payload["rid"] = connId;
	payload["result"] = 1;
	// TODO Media - Should receive MediaInfo
	payload["videoCodec"] = "codec";
	payload["audioCodec"] = "codec";
	payload["encryption_alg"] = "alg";
	payload["encryption_key"] = "key";

	// TODO Media - SESSION_MEDIA_SERVER_CREATE
	Json::Value media;
	media["rid"] = connId;
	media["conferenceSize"] = 2;
	// send media

	// TODO Media - SESSION_MEDIA_SERVER_ADD
	Json::Value media2;
	media2["rid"] = connId;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			media2["cid"] = from;
			media2["clientIp"] = "127.0.0.1";
			media2["name"] = "DooSan";
			sessionControl->sendData(303, payload, from);
			continue;
		}

		media2["cid"] = participant;
		media2["clientIp"] = "127.0.0.1";
		media2["name"] = "DooSanJJANG";
		sessionControl->sendData(301, payload, participant);
	}
	// send media
}

void TelephonyManager::onReject(Json::Value data) {
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

bool TelephonyManager::joinableConference(Json::Value data) {
	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());
	Json::Value payload;

	if (connectionMap.count(connId) == 0) {
		std::cerr << "NO ROOM: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 1;
		sessionControl->sendData(208, payload, from);
		return false;
	}

	Connection conn = connectionMap[connId];
	if (!conn.isOnTime()) {
		std::cerr << "NO TIME: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 2;
		sessionControl->sendData(208, payload, from);
		return false;
	}

	std::list<std::string> conferenceList = conn.getConferenceList();
	std::list<std::string>::iterator it;
	it = std::find(conferenceList.begin(), conferenceList.end(), from);
	if (it == conferenceList.end()) {
		std::cerr << "UNINVITED: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 3;
		sessionControl->sendData(208, payload, from);
		return false;
	}

	return true;
}

// Implement interface
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
		onAnswer(data);
	}
	else if (result == 2) { // 2:REJECT
		onReject(data);
	}
}

void TelephonyManager::handleDisconnect(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string connId(data["rid"].asString());

	Json::Value payload;
	payload["rid"] = connId;

	// TODO Media - SESSION_MEDIA_SERVER_REMOVE
	Json::Value media;
	media["rid"] = connId;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		media["cid"] = participant;
		sessionControl->sendData(305, payload, participant);
	}
	connectionMap.erase(connId);
	// send media
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

void TelephonyManager::handleCreateConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId("CONNECTION_" + std::to_string(connNum++));
	Connection conn(connId, data);

	connectionMap.insert({ connId, conn });
	conferenceDb->update(connId, data); // Add data
	std::cout << "Room Created " << connId << std::endl;

	// TODO Media - SESSION_MEDIA_SERVER_CREATE
	Json::Value media;
	media["rid"] = connId;
	media["conferenceSize"] = conn.getConferenceList().size();

	// send media
}

void TelephonyManager::handleJoinConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	if (!joinableConference(data)) {
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	Connection conn = connectionMap[connId];
	conn.setParticipant(from);

	// TODO Media - SESSION_MEDIA_SERVER_ADD
	Json::Value payload;
	payload["rid"] = connId;
	// TODO Media - Should receive MediaInfo
	payload["videoCodec"] = "codec";
	payload["audioCodec"] = "codec";
	payload["encryption_alg"] = "alg";
	payload["encryption_key"] = "key";

	sessionControl->sendData(208, payload, from);

	// TODO Media - SESSION_MEDIA_SERVER_ADD
	Json::Value media;
	media["rid"] = connId;
	media["cid"] = from;
	media["clientIp"] = "127.0.0.1";
	media["name"] = "DooSan";
}

void TelephonyManager::handleExitConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	Connection conn = connectionMap[connId];
	conn.removeParticipant(from);

	Json::Value payload;
	payload["result"] = 1;

	sessionControl->sendData(209, payload, from);

	// TODO Media - SESSION_MEDIA_SERVER_REMOVE
	Json::Value media;
	media["rid"] = connId;
	media["cid"] = from;
}