#include "Call.h"
#include "Constants.h"

Call::Call() {
	state = CallState::STATE_IDLE;
	conference = false;
	videoQuality = 0;
}

void Call::setCallId(std::string id) {
	callId = id;
}

std::string Call::getCallId() {
	return callId;
}

void Call::setCallState(int state) {
	this->state = state;
}

int Call::getCallState() {
	return state;
}

void Call::setContactId(std::string contactId) {
	callerContactId = contactId;
}

std::string Call::getContactId() {
	return callerContactId;
}

void Call::setConference(bool value) {
	conference = value;
}

bool Call::isConference() {
	return conference;
}

void Call::setVideoCodec(std::string codec) {
	videoCodec = codec;
}
std::string Call::getVideoCodec() {
	return videoCodec;
}
void Call::setAudioCodec(std::string codec) {
	audioCodec = codec;
}
std::string Call::getAudioCodec() {
	return audioCodec;
}
void Call::setEncryptionAlg(std::string alg) {
	encryption_alg = alg;
}
std::string Call::getEncryptionAlg() {
	return encryption_alg;
}
void Call::setEncryptionKey(std::string key) {
	encryption_key = key;
}
std::string Call::getEncryptionKey() {
	return encryption_key;
}
void Call::setVideoQuality(int quality) {
	videoQuality = quality;
}
int Call::getVideoQuality() {
	return videoQuality;
}

void Call::setServerIP(std::string ip) {
	serverIP = ip;
}
std::string Call::getServerIP() {
	return serverIP;
}

void Call::setMediaProperty(Json::Value property) {
	videoCodec = property["videoCodec"].asString();
	audioCodec = property["audioCodec"].asString();
	encryption_alg = property["encryption_alg"].asString();
	encryption_key = property["encryption_key"].asString();
}

Json::Value Call::getMediaProperty() {
	Json::Value media;
	media["rid"] = callId;
	media["videoCodec"] = videoCodec;
	media["audioCodec"] = audioCodec;
	media["encryption_alg"] = encryption_alg;
	media["encryption_key"] = encryption_key;
	return media;
}
