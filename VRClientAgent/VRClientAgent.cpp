#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "Advapi32")

#include "NetClient.h"


#define SLEEP_TIME 1000

NetClient* pNetClient = NULL;


SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;
void ServiceMain(int argc, char** argv);
void ControlHandler(DWORD request);
bool InitService();

int main(int argc, char* argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = L"VRClientAgent";
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;


	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;

	StartServiceCtrlDispatcher(ServiceTable);
}


void ServiceMain(int argc, char** argv)
{
	ServiceStatus.dwServiceType =	SERVICE_WIN32;
	ServiceStatus.dwCurrentState =	SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted =	SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;


	hStatus = RegisterServiceCtrlHandler(	L"MemoryStatus", (LPHANDLER_FUNCTION)ControlHandler);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		// Registering Control Handler failed
		return;
	}


	// Initialize Service 
	bool bRet = InitService();
	if (!bRet)
	{
		// Initialization failed
		ServiceStatus.dwCurrentState =	SERVICE_STOPPED;
		ServiceStatus.dwWin32ExitCode = -1;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	}
	// We report the running status to SCM. 
	ServiceStatus.dwCurrentState =	SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);


	MEMORYSTATUS memory;
	// The worker loop of a service
	while (ServiceStatus.dwCurrentState ==	SERVICE_RUNNING)
	{
		//char buffer[16];
		//GlobalMemoryStatus(&memory);
		//sprintf(buffer, "%d", memory.dwAvailPhys);
		//int result = WriteToLog(buffer);
		//if (result)
		//{
		//	ServiceStatus.dwCurrentState =
		//		SERVICE_STOPPED;
		//	ServiceStatus.dwWin32ExitCode = -1;
		//	SetServiceStatus(hStatus,
		//		&ServiceStatus);
		//	return;
		//}

		if (!(pNetClient->bConnectedToSceneController))
		{
			pNetClient->Disconn();
			delete pNetClient;
			Sleep(SLEEP_TIME);
			pNetClient = new NetClient();
			pNetClient->ReadIniFile();
			pNetClient->ConnectSceneController();
		}
		else
		{
			pNetClient->HandleNetEventFromSceneController();
		}
		Sleep(SLEEP_TIME);
	}
	return;
}

void ControlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		//LOG(info, "Monitoring stopped.");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;


	case SERVICE_CONTROL_SHUTDOWN:
		//LOG(info, "Monitoring stopped.");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;


	default:
		break;
	}

	// Report current status
	SetServiceStatus(hStatus, &ServiceStatus);
	return;
}


bool InitService() {

	//*****开启日志系统*****//
	InitLogger("Log/VRClientAgent");

	pNetClient = new NetClient();
	if (!pNetClient)
	{
		return false;
	}

	int ret = pNetClient->ReadIniFile();
	if (ret != SUCCESS)
	{
		return false;
	}

	pNetClient->ConnectSceneController();

	LOG(info, "VR Client Agent Service start.");

	return true;
}
