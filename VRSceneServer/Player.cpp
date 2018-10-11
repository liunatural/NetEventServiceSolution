#include "Player.h"


Player::Player(LinkID linkID)
{
	mUserType		= VIP;		//新建玩家实例默认指定为VIP客户端玩家类型
	bBoundUser		= false;
	mLinkID			= linkID;
	mPlyID				= linkID;

	mSeatNumber = mProfileInfo.mSeatNumber = 0;
	SetUserState(state_initial);

}


Player::~Player()
{
}

