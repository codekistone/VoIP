#include <iostream>
#include <thread>
#include <chrono>

#include "CallsManager.h"
#include "Constants.h"

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

std::string makeMessage(int msgId, Json::Value payload) {
	Json::Value root;
	root["msgId"] = msgId;
	root["payload"] = payload;

	std::string jsonString = fastWriter.write(root);
	return jsonString;
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

	sessionControl->sendData(makeMessage(301, payload).c_str());
}

void CallsManager::answerCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: answerCall" << std::endl;
	Json::Value payload;
	payload["rid"] = call->getCallId();
	payload["result"] = 1;
	sessionControl->sendData(makeMessage(302, payload).c_str());
}

void CallsManager::rejectCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: rejectCall" << std::endl;
	Json::Value payload;
	payload["rid"] = call->getCallId();
	payload["result"] = 2;
	payload["cuase"] = 1;
	sessionControl->sendData(makeMessage(302, payload).c_str());
}

void CallsManager::disconnectCall() {
	if (sessionControl == nullptr) {
		std::cerr << "Not register sessionControl" << std::endl;
		return;
	}
	std::string callId = call->getCallId();
	std::cout << "disconnectCall... (" << callId << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST

	Json::Value payload;
	payload["rid"] = callId;
	sessionControl->sendData(makeMessage(305, payload).c_str());
}

void CallsManager::onSuccessfulOutgoingCall(Json::Value data) {
	std::string connId(data["rid"].asString());
	call->setCallId(connId);
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulOutgoingCall " << std::endl;
}

void CallsManager::onFailedOutgoingCall(Json::Value data) {
	int cause = data["cause"].asInt();
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onFailedOutgoingCall cause: " << cause << std::endl;
}

void CallsManager::onSuccessfulIncomingCall() {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulIncomingCall" << std::endl;
}

void CallsManager::onRejectedIncomingCall() {
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onRejectedIncomingCall" << std::endl;
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
		payload["cause"] = 2;
		sessionControl->sendData(makeMessage(302, payload).c_str());
		return;
	}
	if (call != NULL) {
		delete call;
	}
	call = new Call();
	call->setCallId(connId);
	call->setContactId(from);
	call->setCallState(CallState::STATE_RINGING);
	std::cout << "[Received] -> (STATE_RINGING) Calling from " << from << " (Answer(5)/Reject(6))" << std::endl;
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
		onSuccessfulIncomingCall();
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
}