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

