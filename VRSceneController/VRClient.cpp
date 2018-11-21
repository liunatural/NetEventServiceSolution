#include "VRClient.h"


VRClient::VRClient(LinkID linkID)
{
	mUserType		= VRClientAgent;
	bBoundUser		= false;
	mLinkID			= linkID;
	mClientID				= linkID;

	mSeatNumber = mProfileInfo.mSeatNumber = 0;
	SetUserState(state_initial);

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

	memset(mUserID, 0, sizeof(mUserID));

	sprintf(mUserID, "%04d", mClientID);

}

