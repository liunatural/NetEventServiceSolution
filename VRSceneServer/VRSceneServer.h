#pragma once
#include "NetEventService.h"
#include "protocol.h"


class PlayerManager;

class VRSceneServer
{
public:
	VRSceneServer();
	virtual ~VRSceneServer();

	int ReadConfigFile();
	int Start();
	int CreatePlayerManager();
	void HandleNetEvent();

	PlayerManager*& GetPlayerManager();

private:

	ConfigService* confReader;
	NetEvtServer* pNetEventServer;
	PlayerManager *playerMgr;

	char sceneServerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

