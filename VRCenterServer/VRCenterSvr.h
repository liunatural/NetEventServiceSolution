#pragma once
#include "NetEventService.h"
#include "protocol.h"

class UserManager;
class VRCenterSvr
{
public:
	VRCenterSvr();
	~VRCenterSvr();

	int ReadConfigFile();
	int Start();
	int CreateUserManager();
	void HandleNetEvent();

	UserManager*& GetUserManager();

private:

	ConfigService* confReader;
	NetEvtServer* pNetEventServer;
	UserManager *userMgr;

};

