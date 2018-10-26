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

	void SetSceneServerID(char* pSceneSvrID) { memcpy(mSceneServerID, pSceneSvrID, SCENE_SERVER_ID_LENGTH); }

	//���û��߻�ȡ�����ķ��������������ӷ���
	NetEvtClient* GetCenterSvrConnection() { return mpConnToCenterSvr; };
	void SetCenterSvrClient(NetEvtClient* pConn) { mpConnToCenterSvr = pConn; };

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

	//�����������
	bool UpdateUserType(int plyId, UserType userType);

	//���û���ģ���ݵ���ϯ��
	void BindFaceModeWithSeatNumber(LinkID& linkID, FaceModel* faceModel, int& plyId);

	//�㲥�û�״̬�仯��Ϣ����-��Ӱ-������״̬��
	void BroadcastUserState(int plyId, int msgID, UserState userState);

	void BroadcastExternalUserState(const MessagePackage* pack);

	//�㲥�û���������
	void BroadcastControlCmd(int msgID, int cmdID);

	//�������λ��
	void UpdatePlayerTransform(int plyId, TransformInfo& transInfo);

	////��������Ƶ��FLV��ʽ�ķ�װ����
	//int SendFlvStream(char* data, int len);

	////��������Ƶ��Sequence_Header����(SPS/PPS��)
	//bool SendFlvSeqHeaderData(Player* user);

	////��������Ƶ��Sequence_Header����
	//void CacheFlvSeqHeaderData(unsigned char* data, int len);


	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

private:
	//����һ�����
	Player* FindPlayer(int plyId);

	void ListPlayer();


	//�������κŲ������ID
	Player* FindPlayerBySeatNumber(int seatNumber);

	//������б���ɾ��һ�����
	bool DeletePlayer(int plyId);

	//��ȡһ��δ����ҵ�VIP�ͻ���
	Player*  GetFreePlayer();

public:
	boost::mutex			mMutex;

private:
	//typedef map<uint8_t, FlvSeqHeader> FlvSeqHeaderMap;
	//FlvSeqHeaderMap flvSeqHeaderMap;	//FLV Sequence Header ����
	//bool flvSeqHeaderChange;

	NetEvtServer *mpService;
	NetEvtClient* mpConnToCenterSvr;
	bool user_visible_flag;						 //�û��Ƿ��ܹ��糡���������ɼ��ı�־
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SCENE_SERVER_ID_LENGTH + 1] = { 0 };
};

