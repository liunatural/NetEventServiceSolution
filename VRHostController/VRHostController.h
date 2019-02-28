//**************************************************************************
//  File......... : VRHostController.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the VRHostController class used as business logics process
//							of VR Host Controller server.
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
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
	int CreateUserSeatMap();
	void Run();

	User_Seat_Map& GetUserSeatMap() { return m_USM; }
	bool CopyData(char* dest, char* source, int len, int max_len);

private:
	void HandleNetEventFromClient();


private:

	char m_HostCtlrID[SERVER_ID_LENGTH + 1]		= { 0 };
	char m_UserID[USER_ID_LENGTH + 1]				= { 0 };

	ConfigService					*m_pConfReader;
	NetEvtServer					*m_pNetEventServer;	
	RemoteClientManager		*m_pClientMgr;
	CSVFile							*m_pCSVFile;
	User_Seat_Map				m_USM;


};

