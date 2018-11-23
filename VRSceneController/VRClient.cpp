#include "VRClient.h"


VRClient::VRClient(LinkID linkID)
{
	m_UserType			= VRClientAgent;
	bBoundUser		= false;
	m_LinkID			= linkID;
}


VRClient::~VRClient()
{
}

void VRClient::SetUserID(char* userid, int len)
{
	
	if (len > USER_ID_LENGTH)
	{
		len = USER_ID_LENGTH;
	}

	memset(m_userInfo.UserID, 0, sizeof(m_userInfo.UserID));
	memcpy(m_userInfo.UserID, userid, len);

}

