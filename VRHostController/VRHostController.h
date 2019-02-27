//**************************************************************************
//  File......... : VRHostController.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the VRHostController class used as business logics process
//							of VR Host Controller server.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************

#pragma once
#include "NetEventService.h"
#include "protocol.h"
#include "CSVFile.h"

class RemoteClientManager;
class VRHostController
{
public:
	VRHostController();
	virtual ~VRHostController();

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
	RemoteClientManager	*clientMgr;
	CSVFile					* m_pCSVFile;
	User_Seat_Map		m_USM;

	char sceneControllerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

