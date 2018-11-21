#pragma once
#include "VRClient.h"
#include "NetEventService.h"
#include <boost/thread/mutex.hpp>

using namespace std;

//������
class VRClientManager : public std::vector<VRClient*>
{
public:
	VRClientManager();
	virtual ~VRClientManager();

	void SetNetworkService(NetEvtServer* pService) { mpService = pService; };

	void AddVRClient(VRClient* client);


	bool DeleteVRClientFromList(int& clientID);

	//�����ն˵���λ��
	bool UpdateSeatNumber(int clientID, int seatNumber);

	//��userID, ͬʱ������ϯ��
	bool BindUserIDToVRClient(char* userid, int len, int &seatNumber);

	//�����û�����
	bool UpdateClientType(int clientID, UserType userType);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

private:
	//����һ���û�
	VRClient* FindVRClient(int clientID);

	//ɾ��һ���û�
	bool DeleteVRClient(int clientID);

	//��ȡһ��δ���û���VR�ͻ���
	VRClient*  GetFreeVRClient();

public:
	boost::mutex			mMutex;

private:
	NetEvtServer *mpService;
};

