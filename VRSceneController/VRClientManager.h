#pragma once
#include "VRClient.h"
#include "NetEventService.h"
#include <vector>
#include <boost/thread/mutex.hpp>

using namespace std;

//������
class VRClientManager : public std::vector<VRClient*>
{
public:
	VRClientManager();
	virtual ~VRClientManager();


	//�����뱾�س��������������ӷ���
	void SetNetworkService(NetEvtServer* pService) { mpService = pService; };

	void AddVRClient(VRClient* ply);


	//�㲥����뿪��Ϣ
	bool DeleteVRClientFromList(int& plyId);

	//�����ն˵���λ��
	bool UpdateSeatNumber(int plyId, int seatNumber);

	//��userID
	bool BindUserIDToVRClient(int plyId, char* userid, int len);

	//�����������
	bool UpdateUserType(int plyId, UserType userType);

	bool UpdateUserTypeByUserID(char* userid, int len, UserType userType);


	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

private:
	//����һ�����
	VRClient* FindVRClient(int plyId);

	//void ListPlayer();


	//�������κŲ������ID
	VRClient* FindVRClientBySeatNumber(int seatNumber);


	VRClient* FindVRClientByUserID(char* userid);

	//������б���ɾ��һ�����
	bool DeleteVRClient(int plyId);

	//��ȡһ��δ����ҵ�VIP�ͻ���
	VRClient*  GetFreePlayer();

public:
	boost::mutex			mMutex;

private:

	NetEvtServer *mpService;
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

