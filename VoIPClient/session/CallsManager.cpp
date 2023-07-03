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

void CallsManager::startOutgoingCall(std::string to) {
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
	sessionControl->sendData(("startOutgoingCall," + to).c_str());
}

void CallsManager::answerCall() {
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: answerCall" << std::endl;
	sessionControl->sendData(("answerCall" + call->getCallId()).c_str());
}

void CallsManager::rejectCall() {
	if (call->getCallState() != CallState::STATE_RINGING) {
		return;
	}
	std::cout << "CALL :: rejectCall" << std::endl;
	sessionControl->sendData(("rejectCall,rejected," + call->getCallId()).c_str());
}

void CallsManager::disconnectCall() {
	std::cout << "disconnectCall... (" << call->getContactId() << ")" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(500)); //TEST
	sessionControl->sendData(("disconnectCall," + call->getCallId()).c_str());
}

// Implement listener
void CallsManager::onIncomingCall(std::string connId, std::string from) {
	if (call != NULL && call->getCallState() != CallState::STATE_IDLE) {
		sessionControl->sendData(("rejectCall,busy," + connId).c_str());
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

void CallsManager::onSuccessfulOutgoingCall(std::string connId) {
	call->setCallId(connId);
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulOutgoingCall " << std::endl;
}

void CallsManager::onSuccessfulIncomingCall() {
	call->setCallState(CallState::STATE_ACTIVE);
	std::cout << "[Received] -> (STATE_ACTIVE) onSuccessfulIncomingCall" << std::endl;
}

void CallsManager::onFailedOutgoingCall(std::string cause) {
	call->setCallState(CallState::STATE_IDLE);
	std::cout << "[Received] -> (STATE_IDLE) onFailedOutgoingCall cause: " << cause << std::endl;
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