#include "Player.h"


Player::Player(LinkID linkID)
{
	m_UserType				= VIP;		//默认为VIP类型
	m_LinkID				= linkID;
	m_SeatNumber		=  -1;
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

	memset(m_UserID, 0, sizeof(m_UserID));
	
	if (len > 0)
	{
		memcpy(m_UserID, userid, len);
	}
}

