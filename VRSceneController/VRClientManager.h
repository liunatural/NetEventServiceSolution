#pragma once
#include "VRClient.h"
#include "NetEventService.h"
#include <vector>
#include <boost/thread/mutex.hpp>

using namespace std;

//管理类
class VRClientManager : public std::vector<VRClient*>
{
public:
	VRClientManager();
	virtual ~VRClientManager();


	//设置与本地场景服务器的连接服务
	void SetNetworkService(NetEvtServer* pService) { mpService = pService; };

	void AddVRClient(VRClient* ply);


	//广播玩家离开消息
	bool DeleteVRClientFromList(int& plyId);

	//更新终端的座位号
	bool UpdateSeatNumber(int plyId, int seatNumber);

	//绑定userID
	bool BindUserIDToVRClient(int plyId, char* userid, int len);

	//更新玩家类型
	bool UpdateUserType(int plyId, UserType userType);

	bool UpdateUserTypeByUserID(char* userid, int len, UserType userType);


	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

private:
	//查找一个玩家
	VRClient* FindVRClient(int plyId);

	//void ListPlayer();


	//根据座椅号查找玩家ID
	VRClient* FindVRClientBySeatNumber(int seatNumber);


	VRClient* FindVRClientByUserID(char* userid);

	//从玩家列表中删除一个玩家
	bool DeleteVRClient(int plyId);

	//获取一个未绑定玩家的VIP客户端
	VRClient*  GetFreePlayer();

public:
	boost::mutex			mMutex;

private:

	NetEvtServer *mpService;
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

