#pragma once

#include "string.h"
#include "protocol.h"
#include "NetEventService.h"
#include <string>

using namespace std;

struct ClientMessage
{
	int clientID;
	char userID[USER_ID_LENGTH + 1] = {0};
	char userIPAddr[IP_ADDR_LENGTH + 1] = {0};
	int seatNumber;
};

class Output_Log
{
public:
	Output_Log() {}
	Output_Log(LOG_TYPE logType, string logStr);
	
	
	LOG_TYPE m_logType;
	string m_logStr;


};



bool CopyData(char* dest, char* source, int len, int max_len);
