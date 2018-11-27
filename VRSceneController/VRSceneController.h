#pragma once
#include "NetEventService.h"
#include "protocol.h"
#include "CSVFile.h"

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

	int CreateUserSeatMap();

private:
	void HandleNetEventFromClient();


private:
	ConfigService			*confReader;
	NetEvtServer			*pNetEventServer;		//net server
	VRClientManager	*clientMgr;
	CSVFile					* m_pCSVFile;
	User_Seat_Map		m_USM;

	char sceneControllerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

