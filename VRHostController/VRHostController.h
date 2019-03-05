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
#include "CSVFile.h"
#include "Util.h"

class CVRHostControllerUIDlg;
class RemoteClientManager;
class RemoteClient;

class VRHostController
{
public:
	VRHostController(CVRHostControllerUIDlg *pUIDlg);
	VRHostController();

	virtual ~VRHostController();

	int ReadConfigFile();
	int Start();
	int CreateVRClientManager();
	int CreateUserSeatMap();
	void Run();

	bool  Stop();
	void SetStopFlag(bool flag) { bStopFlag = flag; }

	User_Seat_Map& GetUserSeatMap() { return m_USM; }
	RemoteClientManager* GetClientCollection() { 	return m_pClientMgr;	}

	int GetPort() { return m_port; }
	int GetMaxLinks() { return m_maxLinks; }

private:
	void HandleNetEventFromClient();
	void SendLogMsg(Output_Log& outLog);

private:

	char m_HostCtlrID[SERVER_ID_LENGTH + 1]		= { 0 };
	char m_UserID[USER_ID_LENGTH + 1]				= { 0 };

	ConfigService					*m_pConfReader;
	NetEvtServer					*m_pNetEventServer;	
	RemoteClientManager		*m_pClientMgr;
	CSVFile							*m_pCSVFile;

	User_Seat_Map				m_USM;
	int									m_port;
	int									m_maxLinks;
	volatile bool					bStopFlag;

	CVRHostControllerUIDlg *m_pVRHostControllerUIDlg;


};

