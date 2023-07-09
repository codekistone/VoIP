#pragma once

#define UWM_UI_CONTROLLER			WM_USER+201

// Response message from Manager -> UI Controller -> Mainframe
#define MSG_RESPONSE_CONNECT		1001 // Server connection
#define MSG_RESPONSE_LOGIN			1002 // Login 
#define MSG_RESPONSE_RESET_PW		1003 // Reset PW
#define MSG_RESPONSE_REGISTER		1004 // Register 
#define MSG_RESPONSE_UPDATE_CONTACT	1005 // Update contact, reset password
#define MSG_RESPONSE_DATA			1006 // Update contacts & conferences
#define MSG_RESPONSE_CALL			1007 // Call and conference status

class IUiController {
public:
	virtual void notify(int type, int result) = 0;
};