#pragma once
#include "NetEventService.h"
#include "protocol.h"



class NetClient
{
public:
	NetClient();
	virtual ~NetClient();

	int ReadIniFile();
	int ConnectSceneController();
	void Run();
	void OnConnectSceneController(int& msgID);
	void HandleNetEventFromSceneController();



private:
	NetEvtClient		*SceneControllerClient;				
	bool					bConnectedToSceneController;

	char m_SceneControllerIP[16] = { 0 };
	char  m_SceneControllerPort[6] = {0};

	const char* m_CfgFile = "./VRAgentConfig.ini";

	char m_UserID[32] = { 0 };
};

