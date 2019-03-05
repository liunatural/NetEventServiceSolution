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

	//����λ��ָ����VR��������
	bool AssignSeatNumToVRHost(int clientID, int seatNumber);

	//����UserSeatMap���ؽ�VR�����������SeatNumber��UserID��ӳ���ϵ
	void RecreateUserSeatMap(RemoteClient* pVRHost);

	//Ϊ�û�����VR������������VR��������ʵ��
	bool AllocateVRHostForUser(char* userid, int len, RemoteClient** ppVRHostObj);

	//����VR����
	bool ReclaimVRHost(int clientID, int seatNumber);

	//���¿ͻ�������
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

