#pragma once

#include <string>
#include "../../json/json.h"

class Call {
private:
	int state;
	std::string callId;
	std::string callerContactId;
	bool conference;
	std::string videoCodec;
	std::string audioCodec;
	std::string encryption_alg;
	std::string encryption_key;
	int videoQuality;
	std::string serverIP;

public:
	Call();
	void setCallId(std::string id);
	std::string getCallId();
	void setCallState(int state);
	int getCallState();
	void setContactId(std::string contactId);
	std::string getContactId();
	void setConference(bool value);
	bool isConference();
	void setVideoCodec(std::string codec);
	std::string getVideoCodec();
	void setAudioCodec(std::string codec);
	std::string getAudioCodec();
	void setEncryptionAlg(std::string alg);
	std::string getEncryptionAlg();
	void setEncryptionKey(std::string key);
	std::string getEncryptionKey();
	void setMediaProperty(Json::Value property);
	Json::Value getMediaProperty();
	void setServerIP(std::string ip);
	std::string getServerIP();

	int getVideoQuality();
	void setVideoQuality(int quality);
};