#pragma once
#include "VRClient.h"
#include "NetEventService.h"
#include <boost/thread/mutex.hpp>

using namespace std;


class VRSceneController;

//管理类
class VRClientManager : public std::vector<VRClient*>
{
public:
	VRClientManager();
	virtual ~VRClientManager();

	void SetNetworkService(NetEvtServer* pService) { mpService = pService; }
	void SetSceneController(VRSceneController* pSceneCtrl) { mpSceneCtrl = pSceneCtrl; }

	void AddVRClient(VRClient* client);


	bool DeleteVRClientFromList(int& clientID);

	//更新终端的座位号
	bool UpdateSeatNumber(int clientID, int seatNumber);

	//根据UserSeatMap表设定userID和绑定状态
	void CheckUserSeatMap(VRClient* pClient);

	//绑定userID, 同时返回座席号
	bool BindUserIDToVRClient(char* userid, int len, VRClient** client);

	//更新用户类型
	bool UpdateClientType(int clientID, UserType userType);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

	bool SendMsg(LinkID& linkID, const MessagePackage& msgPackage);

private:
	//查找一个用户
	VRClient* FindVRClient(int clientID);

	//删除一个用户
	bool DeleteVRClient(int clientID);

	//获取一个未绑定用户的VR客户端
	VRClient*  GetFreeVRClient();

public:
	boost::mutex			mMutex;

private:
	NetEvtServer *mpService;
	
	VRSceneController *mpSceneCtrl;

};

