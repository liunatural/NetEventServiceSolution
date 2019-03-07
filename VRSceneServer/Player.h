#pragma once
#include "protocol.h"
#include  "Message.h"
#include "string.h"

class Player
{
public:
	Player(LinkID linkID);
	virtual ~Player();

	LinkID& GetLinkID() { return m_LinkID; };

	//场景服务器ID
	char* GetSceneServerID() { return m_SceneServerID; }
	void SetSceneServerID(char* pSceneSvrID) { memcpy(m_SceneServerID, pSceneSvrID, SERVER_ID_LENGTH); }

	void SetSeatNumber(int seatNum) { m_SeatNumber = seatNum; }
	int GetSeatNumber() { return m_SeatNumber; }

	void SetUserID(char* userid, int len);
	char* GetUserID() { return m_UserID; }

	void SetUserType(int userType) {	m_UserType = userType;}
	int GetUserType() { return m_UserType; }

	TransformInfo& GetTransformInfo() { return m_TransInfo; }
	void SetTransformInfo(TransformInfo& transInfo) { m_TransInfo = transInfo; }
		
private:
	int							m_SeatNumber;
	int							m_UserType;			//1: VIP客户端， 2: 胶囊客户端

	char						m_UserID[USER_ID_LENGTH + 1]					= { 0 };
	char						m_SceneServerID[SERVER_ID_LENGTH + 1]	= { 0 };
	LinkID					m_LinkID;
	TransformInfo		m_TransInfo;

};