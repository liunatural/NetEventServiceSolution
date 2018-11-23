#pragma once
#include "NetEventService.h"
#include "protocol.h"


class VRClientManager;

class VRSceneController
{
public:
	VRSceneController();
	virtual ~VRSceneController();

	int ReadConfigFile();
	int Start();
	int CreateVRClientManager();
	void Run();

private:
	void HandleNetEventFromClient();


private:
	ConfigService			*confReader;
	NetEvtServer			*pNetEventServer;		//net server
	VRClientManager	*clientMgr;
	char sceneControllerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

