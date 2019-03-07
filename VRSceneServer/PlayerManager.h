#pragma once
#include "Player.h"
#include "NetEventService.h"
#include <vector>
#include <boost/thread/mutex.hpp>

using namespace std;

//玩家管理类
class PlayerManager : public std::vector<Player*>
{
public:
	PlayerManager();
	virtual ~PlayerManager();

	//设置或者获取用户对外的可见性属性
	bool GetUserVisibilityExternal() { return user_visible_flag; }
	void SetUserVisibilityExternal(bool flag) {	user_visible_flag = flag; 	}

	void SetSceneServerID(char* pSceneSvrID) { memcpy(mSceneServerID, pSceneSvrID, SERVER_ID_LENGTH); }

	//设置或者获取与中心服务器的网络连接服务
	NetEvtClient*& GetCenterSvrConnection() { return m_pCenterNetCli; };
	void SetCenterSvrClient(NetEvtClient*& pConn) { m_pCenterNetCli = pConn; };

	//设置与本地场景服务器的连接服务
	void SetNetworkService(NetEvtServer* pService) { m_pLocalNetSvr = pService; };

	void AddPlayer(Player* ply);

	////向外部场景服务器发送本地用户列表
	//void SendClientListToCenterServer();

	//发送玩家离开消息
	bool SendPlayerLeaveMsg(int& plyId);

	void SendUserInfoList(LinkID& linkID);

	//更新用户信息（UserID, Seat Number）
	bool UpdateUserInfo(int plyId, UserInfo *usrInfo);

	//更新玩家类型
	bool UpdateUserType(int plyId, UserType userType);

	bool UpdateUserTypeByUserID(char* userid, int len, UserType userType);

	void BroadcastExternalUserState(const MessagePackage* pack);

	//广播用户控制命令
	void BroadcastControlCmd(int msgID, int cmdID);

	void BroadcastNewUserOnline(int plyId);

	//更新玩家位置
	void UpdatePlayerTransform(int plyId, TransformInfo& transInfo);

	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

	boost::mutex	& GetMutex() {	return m_Mutex;	}

private:
	Player* FindPlayer(int linkID);
	Player* FindPlayerBySeatNumber(int seatNumber);
	Player* FindPlayerByUserID(char* userid);
	bool DeletePlayer(int linkID);


private:
	boost::mutex					m_Mutex;
	NetEvtServer					*m_pLocalNetSvr;
	NetEvtClient					*m_pCenterNetCli;

	bool user_visible_flag;						 //用户是否能够跨场景服务器可见的标志
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SERVER_ID_LENGTH + 1] = { 0 };
};

