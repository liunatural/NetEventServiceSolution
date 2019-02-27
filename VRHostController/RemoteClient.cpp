//**************************************************************************
//  File......... : RemoteClient.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Implementation class file of the RemoteClient.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************

#include "RemoteClient.h"

RemoteClient::RemoteClient(LinkID linkID)
{
	m_ClientType			= VRClientAgent;
	bBoundUser		= false;
	m_LinkID			= linkID;
}


RemoteClient::~RemoteClient()
{
}

void RemoteClient::AssignUserID(char* userid, int len)
{
	
	if (len > USER_ID_LENGTH)
	{
		len = USER_ID_LENGTH;
	}

	memset(m_userInfo.UserID, 0, sizeof(m_userInfo.UserID));
	
	if (len > 0)
	{
		memcpy(m_userInfo.UserID, userid, len);
	}
}

