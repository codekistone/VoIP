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
		std::cout << "cannot establish new call!!" << std::endl;
		return;
	}

	if (call == NULL) {
		call = new Call(to);
	}
	call->setContactId(to);
	call->setCallState(CallState::STATE_DIALING);

	std::cout << "startOutgoingCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("startOutgoingCall");
}

void CallsManager::disconnectCall() {
	std::cout << "disconnectCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData("disconnectCall");
}

// Implement listener
void CallsManager::onSuccessfulOutgoingCall() {
	std::cout << "[Received] -> onSuccessfulOutgoingCall" << std::endl;
}

void CallsManager::startIncomingCall() {
	std::cout << "[Received] -> startIncomingCall" << std::endl;
}

void CallsManager::onSuccessfulIncomingCall() {
	std::cout << "[Received] -> onSuccessfulIncomingCall" << std::endl;
}

void CallsManager::onDisconnected() {
	std::cout << "[Received] -> onDisconnected" << std::endl;
	call->setCallState(CallState::STATE_DISCONNECTED);

	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	call->setCallState(CallState::STATE_IDLE);
}