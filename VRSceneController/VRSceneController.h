//**************************************************************************
//  File......... : VRSceneController.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the VRSceneController class used as business logics process
//							of VR Scene Controller server.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************

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
	User_Seat_Map& GetUserSeatMap() { return m_USM; }

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

