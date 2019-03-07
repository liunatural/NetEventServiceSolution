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
	ConfigService		*m_pConfReader;
	NetEvtServer		*m_pNetEventServer;					//场景服务器 net server
	NetEvtClient		*m_pCenterSvrClient;					//中心服务器 net client
	PlayerManager	*m_pPlayerMgr;
	bool					m_bConnectCenterSvr;
	char					m_SceneServerID[SERVER_ID_LENGTH + 1] = { 0 };
};
