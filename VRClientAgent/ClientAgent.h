#pragma once
#include "NetEventService.h"
#include "protocol.h"



class ClientAgent
{
public:
	ClientAgent();
	virtual ~ClientAgent();

	int ReadIniFile();
	int ConnectHostController();
	void OnConnectHostController(int& msgID);
	void HandleMessage();
	void HandleDeviceStatus();
	void Disconn();

	char* GetIniFilePath() {	return m_CfgFile; }
	int GetSeatNumber() { return m_SeatNumber; }

	NetEvtClient* GetHostCtlrNetClient() {	return m_pHostCtlrNetClient; }

public:
	bool					m_bConnToHostCtlr;

private:
	NetEvtClient		*m_pHostCtlrNetClient;				
	char m_HostControllerIP[IP_ADDR_LENGTH + 1]		= { 0 };
	char m_HostControllerPort[IP_PORT_LENGTH +1]		= { 0 };
	char m_CfgFile[MAX_PATH_LEN + 1]								= { 0 };
	char m_UserID[USER_ID_LENGTH + 1]					= { 0 };

	int m_SeatNumber													= -1;
};

