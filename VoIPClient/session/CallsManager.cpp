#include <iostream>
#include <thread>
#include <chrono>

#include "CallsManager.h"
#include "Constants.h"

#include "../ClientMediaManager.h";

CallsManager* CallsManager::instance = nullptr;
Json::FastWriter fastWriter;

CallsManager::CallsManager() {
	sessionControl = nullptr;
	call = NULL;
}

CallsManager* CallsManager::getInstance() {
	if (instance == nullptr) {
		instance = new CallsManager();
	}
	return instance;
}

void CallsManager::releaseInstance() {
	if (instance != nullptr) {
		instance->setSessionControl(nullptr);
		delete instance;
		instance = nullptr;
		std::cout << "CallsManager::releaseInstance" << std::endl;
	}
}

Call* CallsManager::getCall() {
	return call;
}

void CallsManager::startOutgoingCall(std::string to) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		std::cout << "cannot establish NewCall!!" << std::endl;
		return;
	}

	if (call != NULL) {
		delete call;
	}
	call = new Call();
	call->setContactId(to);
	call->setCallState(CallState::STATE_DIALING);

	std::cout << "(STATE_DIALING) startOutgoingCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST

	Json::Value payload;
	payload["to"] = to;

	sessionControl->sendData(301, payload);
}

void CallsManager::answerCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call == NULL) {
		std::cout << "No call instance" << std::endl;
		return; 
	}
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: answerCall" << std::endl;
	Json::Value payload;
	payload["rid"] = call->getCallId();
	payload["result"] = 1;
	payload["result_detail"] = "ANSWER";
	sessionControl->sendData(302, payload);
}

void CallsManager::rejectCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call == NULL) {
		std::cout << "No call instance" << std::endl;
		return;
	}
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: rejectCall" << std::endl;
	Json::Value payload;
	payload["rid"] = call->getCallId();
	payload["result"] = 2;
	payload["result_detail"] = "REJECT";
	payload["cause"] = 1;
	payload["cause_detail"] = "REJECTED";
	sessionControl->sendData(302, payload);
}

void CallsManager::disconnectCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call == NULL) {
		std::cout << "No call instance" << std::endl;
		return;
	}
	std::string callId = call->getCallId();
	std::cout << "disconnectCall... (" << callId << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST

	Json::Value payload;
	payload["rid"] = callId;
	sessionControl->sendData(305, payload);
}

void CallsManager::onSuccessfulOutgoingCall(Json::Value data) {
	std::string connId(data["rid"].asString());
	call->setServerIP(data["serverIp"].asString());
	call->setCallId(connId);
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulOutgoingCall " << std::endl;

	Json::Value media = call->getMediaProperty();
	media["serverIp"] = call->getServerIP();
	media["myIp"] = data["myIp"].asString();
	ClientMediaManager::getInstance()->startCall(media);
}

void CallsManager::onFailedOutgoingCall(Json::Value data) {
	int cause = data["cause"].asInt();
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onFailedOutgoingCall cause: " << cause << std::endl;
}

void CallsManager::onSuccessfulIncomingCall(Json::Value data) {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulIncomingCall" << std::endl;

	Json::Value media = call->getMediaProperty();
	media["serverIp"] = call->getServerIP();
	media["myIp"] = data["myIp"].asString();
	ClientMediaManager::getInstance()->startCall(media);
}

void CallsManager::onRejectedIncomingCall() {
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onRejectedIncomingCall" << std::endl;
}

void CallsManager::joinConference(std::string callId) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		std::cout << "cannot establish NewCall!!" << std::endl;
		return;
	}

	if (call != NULL) {
		delete call;
	}
	call = new Call();
	call->setCallId(callId);
	call->setCallState(CallState::STATE_DIALING);

	std::cout << "(STATE_DIALING) joinConference... (" << callId << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST

	Json::Value payload;
	payload["rid"] = callId;

	sessionControl->sendData(208, payload);
}

void CallsManager::onSuccessfulJoinConference(Json::Value data) {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulJoinConference" << std::endl;

	Json::Value media = call->getMediaProperty();
	media["serverIp"] = data["serverIp"].asString();
	media["myIp"] = data["myIp"].asString();
	ClientMediaManager::getInstance()->startCall(media);
}

void CallsManager::onFailedJoinConference(Json::Value data) {
	int cause = data["cause"].asInt();
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onFailedJoinConference cause: " << cause << std::endl;
}

void CallsManager::exitConference(std::string callId) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	Json::Value payload;
	payload["rid"] = callId;
	sessionControl->sendData(209, payload);

	Json::Value media;
	media["rid"] = callId;
	ClientMediaManager::getInstance()->endCall(media);

	std::cout << "exitConference... (" << callId << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
}

// Implement interface
void CallsManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void CallsManager::onIncomingCall(Json::Value data) {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}

	std::string connId = data["rid"].asString();
	std::string from = data["cid"].asString();
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		Json::Value payload;
		payload["rid"] = connId;
		payload["result"] = 2;
		payload["result_detail"] = "REJECT";
		payload["cause"] = 2;
		payload["cause_detail"] = "BUSY";
		sessionControl->sendData(302, payload);
		return;
	}
	if (call != NULL) {
		delete call;
	}
	call = new Call();
	call->setCallId(connId);
	call->setServerIP(data["serverIp"].asString());
	call->setContactId(from);
	call->setCallState(CallState::STATE_RINGING);
	std::cout << "[Received] -> (STATE_RINGING) Calling from " << from << std::endl;
}

void CallsManager::onOutgoingCallResult(Json::Value data) {
	int result = data["result"].asInt();
	if (result == 1) { // 1:success
		onSuccessfulOutgoingCall(data);
	}
	else if (result == 2) { // 2:fail
		onFailedOutgoingCall(data);
	}
}

void CallsManager::onIncomingCallResult(Json::Value data) {
	int result = data["result"].asInt();
	if (result == 1) { // 1:success
		onSuccessfulIncomingCall(data);
	}
	else if (result == 2) { // 2:fail
		onRejectedIncomingCall();
	}
}

void CallsManager::onDisconnected(Json::Value data) {
	call->setCallState(CallState::STATE_DISCONNECTED);
	std::cout << "[Received] -> (STATE_DISCONNECTED) onDisconnected" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) Call CLEAR " << std::endl;

	Json::Value media;
	media["rid"] = call->getCallId();
	ClientMediaManager::getInstance()->endCall(media);
}

void CallsManager::onJoinConferenceResult(Json::Value data) {
	int result = data["result"].asInt();

	if (result == 1) {
		onSuccessfulJoinConference(data);
	}
	else if (result == 2) {
		onFailedJoinConference(data);
	}
}

void CallsManager::onExitConference(Json::Value data) {
	call->setCallState(CallState::STATE_DISCONNECTED);
	std::cout << "[Received] -> (STATE_DISCONNECTED) onExitConference" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) ConferenceCall CLEAR " << std::endl;

	Json::Value media;
	media["rid"] = call->getCallId();
	ClientMediaManager::getInstance()->endCall(media);
}

void CallsManager::onVideoQualityChanged(Json::Value data) {
	int quality = data["quality"].asInt();
	call->setVideoQuality(quality);
	ClientMediaManager::getInstance()->setVideoQuality(quality);
}

// Media Interface
void CallsManager::requestVideoQualityChange(int quality) {
	Json::Value payload;
	payload["rid"] = call->getCallId();
	payload["quality"] = quality;
	sessionControl->sendData(401, payload);
}