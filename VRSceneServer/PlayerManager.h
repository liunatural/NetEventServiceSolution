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
	NetEvtClient*& GetCenterSvrConnection() { return m_pCenterNetCli; };
	void SetCenterSvrClient(NetEvtClient*& pConn) { m_pCenterNetCli = pConn; };

	//�����뱾�س��������������ӷ���
	void SetNetworkService(NetEvtServer* pService) { m_pLocalNetSvr = pService; };

	void AddPlayer(Player* ply);

	////���ⲿ�������������ͱ����û��б�
	//void SendClientListToCenterServer();

	//��������뿪��Ϣ
	bool SendPlayerLeaveMsg(int& plyId);

	void SendUserInfoList(LinkID& linkID);

	//�����û���Ϣ��UserID, Seat Number��
	bool UpdateUserInfo(int plyId, UserInfo *usrInfo);

	//�����������
	bool UpdateUserType(int plyId, UserType userType);

	bool UpdateUserTypeByUserID(char* userid, int len, UserType userType);

	void BroadcastExternalUserState(const MessagePackage* pack);

	//�㲥�û���������
	void BroadcastControlCmd(int msgID, int cmdID);

	void BroadcastNewUserOnline(int plyId);

	//�������λ��
	void UpdatePlayerTransform(int plyId, TransformInfo& transInfo);

	bool SendMsg(const MessagePackage& msgPackage);

	bool SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len);

	boost::mutex	& GetMutex() {	return m_Mutex;	}

private:
	Player* FindPlayer(int linkID);
	Player* FindPlayerBySeatNumber(int seatNumber);
	Player* FindPlayerByUserID(char* userid);
	bool DeletePlayer(int linkID);


private:
	boost::mutex					m_Mutex;
	NetEvtServer					*m_pLocalNetSvr;
	NetEvtClient					*m_pCenterNetCli;

	bool user_visible_flag;						 //�û��Ƿ��ܹ��糡���������ɼ��ı�־
	char user_list_buffer[MessagePackage::max_body_length] = { 0 };
	char mSceneServerID[SERVER_ID_LENGTH + 1] = { 0 };
};

