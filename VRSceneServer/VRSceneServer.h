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
	int ConnectCenterSvr();
	void Run();
	PlayerManager*& GetPlayerManager();
	void OnConnectCenterServer(int& msgID);

private:
	void HandleNetEventFromClient();
	void HandleNetEventFromCenterSvr();


private:
	ConfigService		*confReader;
	NetEvtServer		*pNetEventServer;
	NetEvtClient		*centerSvrClient;
	bool					bConnectCenterSvr;
	PlayerManager	*playerMgr;
	char sceneServerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

