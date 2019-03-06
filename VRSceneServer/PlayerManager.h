#pragma once
#include "Player.h"
#include "NetEventService.h"
#include <vector>
#include <boost/thread/mutex.hpp>

using namespace std;

//��ҹ�����
class PlayerManager : public std::vector<Player*>
{
public:
	PlayerManager();
	virtual ~PlayerManager();

	//���û��߻�ȡ�û�����Ŀɼ�������
	bool GetUserVisibilityExternal() { return user_visible_flag; }
	void SetUserVisibilityExternal(bool flag) {	user_visible_flag = flag; 	}

	void SetSceneServerID(char* pSceneSvrID) { memcpy(mSceneServerID, pSceneSvrID, SERVER_ID_LENGTH); }

	//���û��߻�ȡ�����ķ��������������ӷ���
	NetEvtClient*& GetCenterSvrConnection() { return mpConnToCenterSvr; };
	void SetCenterSvrClient(NetEvtClient*& pConn) { mpConnToCenterSvr = pConn; };

	//�����뱾�س��������������ӷ���
	void SetNetworkService(NetEvtServer* pService) { mpService = pService; };

	void AddPlayer(Player* ply);

	//�򱾵��û����͵�ǰ���߲����Ѿ�������ϯ�ŵ�VIP�ͻ����б���Ϣ
	void SendClientList(LinkID& linkID);

	//���ⲿ�������������ͱ����û��б�
	void SendClientListToCenterServer();

	//�㲥����ĳ�������׼������Ϣ
	void SendUserReadyMsg(int plyId);

	//�㲥����뿪��Ϣ
	bool SendPlayerLeaveMsg(int& plyId);

	//������ҵ���λ��
	bool UpdatePlayerSeatNumber(int plyId, int seatNumber);

	//��userID
	bool BindUserIDToPlayer(int plyId, char* userid, int len);

	void SendUserInfoList(LinkID& linkID);
	bool UpdateUserInfo(int plyId, UserInfo *usrInfo);


	//�����������
	bool UpdateUserType(int plyId, UserType userType);

	bool UpdateUserTypeByUserID(char* userid, int len, UserType userType);


	//���û���ģ���ݵ���ϯ��
	void BindFaceModeWithSeatNumber(LinkID& linkID, FaceModel* faceModel, int& plyId);

	//�㲥�û�״̬�仯��Ϣ����-��Ӱ-������״̬��
	void BroadcastUserState(int plyId, int msgID, UserState userState);

	void BroadcastExternalUserState(const MessagePackage* pack);

	//�㲥�û���������
	void BroadcastControlCmd(int msgID, int cmdID);

	void BroadcastNewUserOnline(int plyId);


	//�������λ��
	void UpdatePlayerTransform(int plyId, TransformInfo& transInfo);

	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

private:
	//����һ�����
	Player* FindPlayer(int plyId);

	void ListPlayer();


	//�������κŲ������ID
	Player* FindPlayerBySeatNumber(int seatNumber);


	Player* FindPlayerByUserID(char* userid);

	//������б���ɾ��һ�����
	bool DeletePlayer(int plyId);

	//��ȡһ��δ����ҵ�VIP�ͻ���
	Player*  GetFreePlayer();

public:
	boost::mutex			mMutex;

private:

	NetEvtServer *mpService;
	NetEvtClient* mpConnToCenterSvr;
	bool user_visible_flag;						 //�û��Ƿ��ܹ��糡���������ɼ��ı�־
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SERVER_ID_LENGTH + 1] = { 0 };
};

