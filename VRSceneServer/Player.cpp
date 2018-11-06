#include "Player.h"


Player::Player(LinkID linkID)
{
	mUserType		= VIP;		//�½����ʵ��Ĭ��ָ��ΪVIP�ͻ����������
	bBoundUser		= false;
	mLinkID			= linkID;
	mPlyID				= linkID;

	mSeatNumber = mProfileInfo.mSeatNumber = 0;
	SetUserState(state_initial);

}


Player::~Player()
{
}

void Player::SetUserID(char* userid, int len)
{
	if (len > USER_ID_LENGTH)
	{
		len = USER_ID_LENGTH;
	}

	memset(mUserID, 0, sizeof(mUserID));
	//memcpy(mUserID, userid, len);

	sprintf(mUserID, "%04d", mPlyID);

}

