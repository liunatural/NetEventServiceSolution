#pragma once
#include "User.h"
#include "NetEventService.h"
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

using namespace std;

//用户管理类
class UserManager : public std::vector<User*>
{
public:
	UserManager();
	virtual ~UserManager();

	void AddUser(User* ply);
	bool DeleteUser(int plyId);
	void SetNetworkService(NetEvtServer** pService);
	
	//发送简单的消息
	int SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);
	
	int ForwardCommand(MessagePackage *pkt);
	int ForwardMsgToOtherSceneServers(LinkID& sourceLinkID, MessagePackage *pkt);

private:
	User* FindUser(int plyId);

public:
	boost::mutex			mMutex;

private:
	NetEvtServer *mpService;

};

