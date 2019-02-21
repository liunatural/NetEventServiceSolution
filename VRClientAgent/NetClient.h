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
	//void Run();
	void OnConnectSceneController(int& msgID);
	void HandleNetEventFromSceneController();
	void HandleDeviceStatus();
	void Disconn();

	char* GetIniFilePath() {	return m_CfgFile; }
	int GetSeatNumber() { return m_SeatNumber; }

	NetEvtClient* GetConnectToSceneController() {	return SceneControllerClient; }

public:
	bool					bConnectedToSceneController;

private:
	NetEvtClient		*SceneControllerClient;				


	char m_SceneControllerIP[16] = { 0 };
	char  m_SceneControllerPort[6] = {0};

	int m_SeatNumber = -1;
	char m_CfgFile[MAX_PATH] = { 0 };

	char m_UserID[32] = { 0 };
};

