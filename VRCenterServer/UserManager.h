#pragma once
#include "User.h"
#include "NetEventService.h"
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

using namespace std;

//�û�������
class UserManager : public std::vector<User*>
{
public:
	UserManager();
	virtual ~UserManager();

	void AddUser(User* ply);
	bool DeleteUser(int plyId);
	void SetNetworkService(NetEvtServer** pService);
	
	//���ͼ򵥵���Ϣ
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

