//**************************************************************************
//  File......... : RemoteClient.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the Remote Client class. 
//
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
//***************************************************************************
#pragma once
#include  "Message.h"
#include "Util.h"


class RemoteClient
{
public:
	RemoteClient(LinkID linkID);
	virtual ~RemoteClient();

	LinkID& GetLinkID() { return m_LinkID; };

	void SetClientType(int clientType) { m_ClientType = clientType; }
	int GetClientType() { return m_ClientType; }

	void SetSeatNumber(int seatNum) { m_userInfo.SeatNumber  = seatNum; }
	int GetSeatNumber() { return m_userInfo.SeatNumber; }

	void AssignUserID(char* userid, int len);

	UserInfo* GetUserInfo() { return &m_userInfo; }

	bool IsBoundUser() { return bBoundUser; }
	void SetUserBindFlag(bool flag) { bBoundUser = flag; }

	void SaveIP(char* ipAddr, int len);
	char* GetIP() { return m_IP; }

private:
	LinkID		m_LinkID;
	UserInfo	m_userInfo;
	bool			bBoundUser;
	int				m_ClientType;			//7: VR终端代理， 2: 胶囊客户端

	char			m_IP[IP_ADDR_LENGTH] = { 0 };
};