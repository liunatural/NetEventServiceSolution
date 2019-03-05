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

class RemoteClientManager : public std::vector<RemoteClient*>
{
public:
	RemoteClientManager();
	virtual ~RemoteClientManager();

	void SetNetworkService(NetEvtServer* pService) { m_pNetEvSvc = pService; }
	void SetHostController(VRHostController* pHostCtlr) { m_pHostCtlr = pHostCtlr; }

	void AddRemoteClient(RemoteClient* client);
	bool DeleteRemoteClient(int& clientID);

	//将座位号指定给VR主机对象
	bool AssignSeatNumToVRHost(int clientID, int seatNumber);

	//根据UserSeatMap表重建VR主机对象里的SeatNumber和UserID的映射关系
	void RecreateUserSeatMap(RemoteClient* pVRHost);

	//为用户分配VR主机，并返回VR主机对象实例
	bool AllocateVRHostForUser(char* userid, int len, RemoteClient** ppVRHostObj);

	//回收VR主机
	bool ReclaimVRHost(int clientID, int seatNumber);

	//更新客户端类型
	bool UpdateClientType(int clientID, UserType userType);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);
	bool SendMsg(LinkID& linkID, const MessagePackage& msgPackage);
	RemoteClient* FindClient(int clientID);
	RemoteClient*  GetFreeRemoteClient();

	boost::mutex&	GetMutex() {	return m_RCMgrMutex;	}
	Output_Log&		GetOutputLog() { return m_outLog; }
private:
	boost::mutex			m_RCMgrMutex;
	NetEvtServer			*m_pNetEvSvc;
	VRHostController	*m_pHostCtlr;
	Output_Log			m_outLog;

	char msgBuf[1024] = { 0 };



};

