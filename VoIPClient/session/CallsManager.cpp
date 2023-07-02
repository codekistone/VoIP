#include <iostream>
#include <thread>
#include <chrono>

#include "CallsManager.h"
#include "Constants.h"

CallsManager* CallsManager::instance = nullptr;

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

void CallsManager::setSessionControl(SessionControl* control) {
	sessionControl = control;
}

void CallsManager::startOutgoingCall(const char* to) {
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		std::cout << "cannot establish NewCall!!" << std::endl;
		return;
	}

	if (call == NULL) {
		call = new Call(to);
	}
	call->setContactId(to);
	call->setCallState(CallState::STATE_DIALING);

	std::cout << "(STATE_DIALING) startOutgoingCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("startOutgoingCall");
}

void CallsManager::answerCall() {
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "answerCall" << std::endl;
	sessionControl->sendData("answerCall");
}

void CallsManager::rejectCall() {
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "rejectCall" << std::endl;
	sessionControl->sendData("rejectCall");
}

void CallsManager::disconnectCall() {
	std::cout << "disconnectCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("disconnectCall");
}

// Implement listener
void CallsManager::onIncomingCall(const char* from) {
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		std::cout << "cannot establish NewCall!!" << std::endl;
		return;
	}
	if (call == NULL) {
		call = new Call(from);
	}
	call->setContactId(from);
	call->setCallState(CallState::STATE_RINGING);
	std::cout << "[Received] -> (STATE_RINGING) Calling from " << from << " (Answer/Reject)" << std::endl;
}

void CallsManager::onSuccessfulOutgoingCall() {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulOutgoingCall " << std::endl;
}

void CallsManager::onSuccessfulIncomingCall() {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulIncomingCall" << std::endl;
}

void CallsManager::onFailedOutgoingCall() {
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onFailedOutgoingCall" << std::endl;
}

void CallsManager::onRejectedIncomingCall() {
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onRejectedIncomingCall" << std::endl;
}

void CallsManager::onDisconnected() {
	call->setCallState(CallState::STATE_DISCONNECTED);
	std::cout << "[Received] -> (STATE_DISCONNECTED) onDisconnected" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) Call CLEAR " << std::endl;
}