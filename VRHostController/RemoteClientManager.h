//**************************************************************************
//  File......... : RemoteClientManager.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Head file of the RemoteClientManager class used as the remote client management
//						
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
//***************************************************************************
#pragma once
#include "RemoteClient.h"
#include "NetEventService.h"
#include <boost/thread/mutex.hpp>

using namespace std;


class VRHostController;

//管理类
class RemoteClientManager : public std::vector<RemoteClient*>
{
public:
	RemoteClientManager();
	virtual ~RemoteClientManager();

	void SetNetworkService(NetEvtServer* pService) { mpService = pService; }
	void SetSceneController(VRHostController* pSceneCtrl) { mpSceneCtrl = pSceneCtrl; }

	void AddRemoteClient(RemoteClient* client);
	bool DeleteRemoteClient(int& clientID);

	//将座位号指定给远程终端
	bool AssignSeatNumberToClient(int clientID, int seatNumber);

	//根据UserSeatMap表重建客户端的SeatNumber和UserID的映射关系
	void RecreateUserSeatMap(RemoteClient* pClient);

	bool ReclaimRemoteClient(int clientID, int seatNumber);

	//绑定userID到客户端对象上
	bool BindUserIDToRemoteClient(char* userid, int len, RemoteClient** ppClient);

	//更新客户端类型
	bool UpdateClientType(int clientID, UserType userType);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

	bool SendMsg(LinkID& linkID, const MessagePackage& msgPackage);

private:
	//查找一个用户
	RemoteClient* FindClient(int clientID);

	////删除一个用户
	//bool DeleteVRClient(int clientID);

	//获取一个未绑定用户的远程客户端对象
	RemoteClient*  GetFreeRemoteClient();

public:
	boost::mutex			mMutex;

private:
	NetEvtServer *mpService;
	
	VRHostController *mpSceneCtrl;

};

