//**************************************************************************
//  File......... : NetEvtService.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : implementaion file of the class NetEvtService used to
//
//  History...... : first created by Liu Zhi 2018-09
//
//**************************************************************************


#include "NetEventService.h"
#include "NetEventServer.h"
#include "NetEventClient.h"
#include "SLog.h"
#include "ConfigReader.h"

NETEVENTSERVICE_API NetEvtServer* CreateNetEvtServer()
{
	NetEventServer* pNetEvServer = new NetEventServer();
	return pNetEvServer;
}

NETEVENTSERVICE_API NetEvtClient* CreateNetEvtClient()
{
	NetEventClient* pNetEvClient = new NetEventClient();
	return pNetEvClient;
}


//Log API
NETEVENTSERVICE_API void InitLogger(const char* dir)
{
	return  SLog::Init(dir);
	
}

NETEVENTSERVICE_API void LOG(LOG_TYPE logType, const char* format, ...)
{
	char temp[MAX_LOG_TEXT_LENGTH];
	va_list vArgList;
	va_start(vArgList, format);
	int len = _vsnprintf(temp, MAX_LOG_TEXT_LENGTH, format, vArgList);
	va_end(vArgList);

	if (len < 0 || len >= MAX_LOG_TEXT_LENGTH)
		return;

	return SLog::Log(logType, temp);
}


//ConfigReader service
NETEVENTSERVICE_API ConfigService* CreateConfigReader()
{
	return new ConfigReader();
}