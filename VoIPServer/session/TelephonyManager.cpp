#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

#include "TelephonyManager.h"
#include "../DumpMediaManager.h"

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
	media["myIp"] = payload["myIp"].asString();
	// send media
	DumpMediaManager::getInstance()->startCall(media);

	// TODO Media - SESSION_MEDIA_SERVER_ADD
	Json::Value clientMedia;
	clientMedia["rid"] = connId;

	Connection conn = connectionMap[connId];
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		if (participant == from) {
			clientMedia["cid"] = from;
			clientMedia["clientIp"] = "127.0.0.1";
			clientMedia["name"] = "DooSan";
			sessionControl->sendData(303, payload, from);
			// send media
			DumpMediaManager::getInstance()->addClient(clientMedia);
			continue;
		}

		clientMedia["cid"] = participant;
		clientMedia["clientIp"] = "127.0.0.1";
		clientMedia["name"] = "DooSanJJANG";
		sessionControl->sendData(301, payload, participant);
		// send media
		DumpMediaManager::getInstance()->addClient(clientMedia);
	}
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

int TelephonyManager::joinableConference(Json::Value data) {
	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());
	Json::Value payload;

	if (connectionMap.count(connId) == 0) {
		return 1;
	}

	Connection conn = connectionMap[connId];
	if (!conn.isOnTime()) {
		return 2;
	}

	std::list<std::string> conferenceList = conn.getConferenceList();
	std::list<std::string>::iterator it = std::find(conferenceList.begin(), conferenceList.end(), from);
	if (it == conferenceList.end()) {
		return 3;
	}

	return 0;
}

void TelephonyManager::manageConferenceLifetime(std::string connId) {
	std::string displayConn = "[" + connId + "]";
	Connection conn = connectionMap[connId];
	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::time_point(std::chrono::seconds(conn.getConferenceStartTime()));
	std::chrono::seconds duration(conn.getDuration());

	while (true) {
		std::time_t now;
		std::time(&now);
		char time[26];
		ctime_s(time, 26, &now);
		std::cout << displayConn << "���� �ð�: " << time << endl;

		// 1�� ���
		std::this_thread::sleep_for(std::chrono::minutes(1));
		std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();

		// timeover
		if (currentTime >= startTime + duration) {
			break;
		}
	}
	std::cout << displayConn << "connection Closed" << endl;
	removeConference(connId);
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

	// TODO Media - SESSION_MEDIA_SERVER_REMOVE
	Json::Value clientMedia;
	clientMedia["rid"] = connId;

	Json::Value payload;
	payload["rid"] = connId;
	Connection conn = connectionMap[connId];
	int msgId;
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		clientMedia["cid"] = participant;
		msgId = conn.isConference() ? 209 : 305;
		sessionControl->sendData(msgId, payload, participant);
		// send media
		DumpMediaManager::getInstance()->removeClient(clientMedia);
	}
	if (!conn.isConference()) {
		connectionMap.erase(connId);
	}
	Json::Value media;
	media["rid"] = connId;
	DumpMediaManager::getInstance()->endCall(media);
}

void TelephonyManager::releaseConnection(std::string cid) {
	Connection conn;
	std::string connId;
	// find connection
	for (const auto& iter : connectionMap) {
		conn = iter.second;
		std::list<std::string> participants = conn.getParticipants();
		if (std::count(participants.begin(), participants.end(), cid) > 0) {
			connId = iter.first;
			break;
		}
	}
	if (!connId.empty() && !conn.isConference()) {
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
	std::cout << "Conference Room Created " << connId << std::endl;
	std::thread room(&TelephonyManager::manageConferenceLifetime, instance, connId);
	room.detach();

	// TODO Media - SESSION_MEDIA_SERVER_CREATE
	Json::Value media;
	media["rid"] = connId;
	media["conferenceSize"] = conn.getConferenceList().size();
	media["myIp"] = data["myIp"].asString();
	// send media
	DumpMediaManager::getInstance()->startCall(media);
}

void TelephonyManager::removeConference(std::string connId) {
	Json::Value data;
	data["rid"] = connId;
	handleDisconnect(data);
	conferenceDb->remove(connId);
	connectionMap.erase(connId);
}

void TelephonyManager::handleJoinConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	int joinable = joinableConference(data);
	Json::Value payload;

	if (joinable == 1) {
		std::cerr << "NO ROOM: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 1;
		sessionControl->sendData(208, payload, from);
		return;
	}
	else if (joinable == 2) {
		std::cerr << "NO TIME: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 2;
		sessionControl->sendData(208, payload, from);
	}
	else if (joinable == 3) {
		std::cerr << "UNINVITED: " << connId << std::endl;
		payload["result"] = 2;
		payload["cause"] = 3;
		sessionControl->sendData(208, payload, from);
	}

	connectionMap[connId].setParticipant(from);

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
	// send media
	DumpMediaManager::getInstance()->addClient(media);
}

void TelephonyManager::handleExitConference(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId(data["rid"].asString());
	std::string from(data["from"].asString());

	connectionMap[connId].removeParticipant(from);

	Json::Value payload;
	payload["result"] = 1;

	sessionControl->sendData(209, payload, from);

	// TODO Media - SESSION_MEDIA_SERVER_REMOVE
	Json::Value media;
	media["rid"] = connId;
	media["cid"] = from;
	// send media
	DumpMediaManager::getInstance()->removeClient(media);
}

void TelephonyManager::handleRequestVideoQualityChange(Json::Value data) {
	//DumpMediaManager::getInstance->updateClientVideoQuality(data);
}

void TelephonyManager::notifyVideoQualityChanged(std::string rid, int quality) {
	// send to client (request quality change)
	Connection conn = connectionMap[rid];

	Json::Value payload;
	payload["quality"] = quality;
	std::list<std::string> participants = conn.getParticipants();
	for (const auto& participant : participants) {
		sessionControl->sendData(402, payload, participant);
	}
}