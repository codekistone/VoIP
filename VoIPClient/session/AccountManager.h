#pragma once

#include "IAccountManager.h"
#include "SessionControl.h"
#include <list>
#include "../../json/json.h"

class AccountManager : public IAccountManager {
private:
	static AccountManager* instance;

	SessionControl* sessionControl;

	AccountManager();

public:
	static AccountManager* getInstance();
	static void releaseInstance();

	struct ContactData {
		std::string cid;
		std::string email;
		std::string name;
	};
	
	void login_();
	void registerAccount(std::string id, std::string email, std::string pw, std::string name, int pwdQuestion, std::string pwdAnswer);
	void login(std::string id, std::string pw);
	void logout(std::string cid);
	void updateMyContactList(std::string cid, std::list<std::string>& list);
	void resetPassword(std::string cid, std::string newPassword, int pwdQuestion, std::string pwdAnswer);
	void getAllContact(std::string cid);

	// Listener
	void setSessionControl(SessionControl* control) override;
	void onLoginSuccess(std::string contactId) override;
	void handleRegisterContact(Json::Value data) override;
	void handleLogin(Json::Value data) override;
	void handleResetPassword(Json::Value data) override;
	void handleGetAllContact(Json::Value data) override;
};
