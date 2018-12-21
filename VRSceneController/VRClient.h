//**************************************************************************
//  File......... : VRClient.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the VRClient class. 
//
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************
#pragma once
#include "protocol.h"
#include  "Message.h"
#include "string.h"

class VRClient
{
public:
	VRClient(LinkID linkID);
	virtual ~VRClient();

	LinkID& GetLinkID() { return m_LinkID; };

	void SetSeatNumber(int seatNum) { m_userInfo.SeatNumber  = seatNum; }
	int GetSeatNumber() { return m_userInfo.SeatNumber; }

	void SetUserID(char* userid, int len);

	void SetUserType(int userType) {	m_UserType = userType;}
	int GetUserType() { return m_UserType; }
	bool HasBoundUser() { return bBoundUser; }
	void SetBoundUser(bool flag) { bBoundUser = flag; }
private:
	UserInfo	m_userInfo;
	LinkID		m_LinkID;
	int				m_UserType;			//7: VR终端代理， 2: 胶囊客户端
	bool			bBoundUser;
};