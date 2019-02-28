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

//������
class RemoteClientManager : public std::vector<RemoteClient*>
{
public:
	RemoteClientManager();
	virtual ~RemoteClientManager();

	void SetNetworkService(NetEvtServer* pService) { mpService = pService; }
	void SetSceneController(VRHostController* pSceneCtrl) { mpSceneCtrl = pSceneCtrl; }

	void AddRemoteClient(RemoteClient* client);
	bool DeleteRemoteClient(int& clientID);

	//����λ��ָ����Զ���ն�
	bool AssignSeatNumberToClient(int clientID, int seatNumber);

	//����UserSeatMap���ؽ��ͻ��˵�SeatNumber��UserID��ӳ���ϵ
	void RecreateUserSeatMap(RemoteClient* pClient);

	bool ReclaimRemoteClient(int clientID, int seatNumber);

	//��userID���ͻ��˶�����
	bool BindUserIDToRemoteClient(char* userid, int len, RemoteClient** ppClient);

	//���¿ͻ�������
	bool UpdateClientType(int clientID, UserType userType);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

	bool SendMsg(LinkID& linkID, const MessagePackage& msgPackage);

private:
	//����һ���û�
	RemoteClient* FindClient(int clientID);

	////ɾ��һ���û�
	//bool DeleteVRClient(int clientID);

	//��ȡһ��δ���û���Զ�̿ͻ��˶���
	RemoteClient*  GetFreeRemoteClient();

public:
	boost::mutex			mMutex;

private:
	NetEvtServer *mpService;
	
	VRHostController *mpSceneCtrl;

};

