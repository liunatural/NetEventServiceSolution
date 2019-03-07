#include "VRSceneServer.h"
#include "PlayerManager.h"
#include <direct.h>
#include <thread>


VRSceneServer* g_pSceneSvr = NULL;

void __stdcall  OnConnectEvent(int& msgID)
{
	if (g_pSceneSvr)
	{
		return g_pSceneSvr->OnConnectCenterServer(msgID);
	}
}

VRSceneServer::VRSceneServer()
{
	m_pConfReader = NULL;
	m_pNetEventServer = NULL;
	m_pPlayerMgr = NULL;

	m_bConnectCenterSvr = false;
	m_pCenterSvrClient = NULL;
}

VRSceneServer::~VRSceneServer()
{
	if (m_pConfReader)
	{
		delete m_pConfReader;
		m_pConfReader = NULL;
	}

	if (m_pNetEventServer)
	{
		delete m_pNetEventServer;
		m_pNetEventServer = NULL;
	}

	if (m_pPlayerMgr)
	{
		delete m_pPlayerMgr;
		m_pPlayerMgr = NULL;
	}
}

int VRSceneServer::ReadConfigFile()
{
	m_pConfReader = CreateConfigReader();
	char path[MAX_PATH_LEN] = { 0 };
	_getcwd(path, MAX_PATH_LEN);
	strcat(path, "/VRSceneServer.xml");
	bool ret = m_pConfReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "�����ļ���ȡʧ��\n");
		return FAIL;
	}
}

int VRSceneServer::Start()
{
	
	if (!m_pConfReader)
	{
		LOG(error, "�����ļ�����confReaderΪNULL��");
		return FAIL;
	}
	
	int port = m_pConfReader->GetInt("root.Server.port");
	int maxlinks = m_pConfReader->GetInt("root.Server.maxlinks");
	m_pConfReader->GetStr("root.Server.name", m_SceneServerID);

	m_pNetEventServer = CreateNetEvtServer();

	bool bRet = m_pNetEventServer->Start(port, maxlinks);
	if (bRet == false)
	{
		return FAIL;
	}

	LOG(info, "���������������ɹ���");

	return SUCCESS;
}


int VRSceneServer::ConnectCenterSvr()
{
	char centerServerIP[50] = { 0 };
	char centerServerPort[50] = { 0 };
	m_pConfReader->GetStr("root.CenterServer.ip", centerServerIP);
	m_pConfReader->GetStr("root.CenterServer.port", centerServerPort);
	m_pCenterSvrClient = CreateNetEvtClient();
	m_pCenterSvrClient->SetEventCallback(OnConnectEvent);
	if (m_pCenterSvrClient->Connect(centerServerIP, centerServerPort) != 0)
	{
		LOG(error, "��ʼ�����ķ���������ʱ���ִ���!");
		return ERR_CONNECT_CENTER_SERVER;
	}

	LOG(info, "���ڿ�ʼ�����ķ���������......");
	m_pCenterSvrClient->Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(2));

	return SUCCESS;
}


int VRSceneServer::CreatePlayerManager()
{

	if (!m_pNetEventServer || strlen(m_SceneServerID) == 0)
	{
		LOG(error, "�����û�������ʧ�ܣ�");
		return FAIL;
	}

	m_pPlayerMgr = new PlayerManager();

	m_pPlayerMgr->SetNetworkService(m_pNetEventServer);
	m_pPlayerMgr->SetSceneServerID(m_SceneServerID);

	return SUCCESS;
}


void VRSceneServer::Run()
{
	while (true)
	{
		HandleNetEventFromClient();

		HandleNetEventFromCenterSvr();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void VRSceneServer::OnConnectCenterServer(int& msgID)
{
	if (link_connected == msgID)
	{
		LOG(info, "�������ķ������ɹ���");
		m_bConnectCenterSvr = true;
		m_pPlayerMgr->SetCenterSvrClient(m_pCenterSvrClient);
	}
	else if (link_server_closed == msgID)
	{
		LOG(error, "���ķ������˶Ͽ������ӣ�");
	}
	else if (link_server_failed == msgID)
	{
		LOG(info, "�������ķ�����û�гɹ���");
	}
}


void VRSceneServer::HandleNetEventFromClient()
{

	MsgQueue& msgQ = m_pNetEventServer->GetMsgQueue();
	int msgAmount = msgQ.GetCount();

	for (int i = 0; i < msgAmount; i++)
	{
		MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

		int msgID = pack->header()->id1;
		int cmdID = pack->header()->id2;
		int cid = pack->GetLinkID();

		switch (msgID)
		{
		case link_connected:
		{

			Player*  ply = new Player(cid);
			ply->SetSceneServerID(m_SceneServerID);
			m_pPlayerMgr->AddPlayer(ply);

			m_pPlayerMgr->SendCmd(cid, link_connected, 0, NULL, 0);
			m_pPlayerMgr->SendUserInfoList(cid);

			break;
		}
		case  link_disconnected:
		{
			m_pPlayerMgr->SendPlayerLeaveMsg(cid);
			break;
		}
		case ID_User_Login:
		{
			if (c2s_tell_user_info == cmdID)
			{
				UserInfo *usrInfo = (UserInfo*)(pack->body());

				//�����û���Ϣ
				bool bRet = m_pPlayerMgr->UpdateUserInfo(cid, usrInfo);
				if (bRet)
				{
					//������VIP�ͻ��˹㲥�û���¼
					m_pPlayerMgr->BroadcastNewUserOnline(cid);
				}
			}
			break;
		}
		case ID_User_Transform:		//���λ�ñ任��Ϣ
		{
			TransformInfo* transInfo = (TransformInfo*)pack->body();
			transInfo->plyId = cid;

			m_pPlayerMgr->UpdatePlayerTransform(cid, *transInfo);

			break;
		}
		case ID_Global_Notify:
		{

			if (cmdID == c2s_seen_external)
			{
				
				char* userid = pack->body();
				int userid_len = pack->GetBodyLength();
				
				bool bRet = m_pPlayerMgr->UpdateUserTypeByUserID(userid, userid_len, ExternalVIP);
				if (bRet)
				{
					m_pCenterSvrClient->Send(ID_Global_Notify, s2c_trans_ext_usr_profile, (const char*)pack->body(), pack->GetBodyLength());
				}
			}
			else if(cmdID == c2s_stop_seen_external)
			{

				char* userid = pack->body();
				int userid_len = pack->GetBodyLength();

				bool bRet = m_pPlayerMgr->UpdateUserTypeByUserID(userid, userid_len, VIP);
				if (bRet)
				{
					m_pCenterSvrClient->Send(ID_Global_Notify, s2c_trans_ext_usr_profile, (const char*)pack->body(), pack->GetBodyLength());
				}
			}
			break;
		}
		case ID_Global_Transform:
		{
			m_pPlayerMgr->SendMsg(*pack);
			break;
		}
		default:
		{
			break;
		}

		} //switch
	}//for
}


void VRSceneServer::HandleNetEventFromCenterSvr()
{
	if (m_bConnectCenterSvr)
	{
		MsgQueue& msgQ = m_pCenterSvrClient->GetMsgQueue();
		int msgAmount = msgQ.GetCount();

		for (int i = 0; i < msgAmount; i++)
		{
			MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

			int msgID = pack->header()->id1;
			int cid = pack->GetLinkID();

			switch (msgID)
			{
			case ID_Global_Notify:
			{
				int cmdID = pack->header()->id2;
				if (s2c_trans_ext_usr_profile == cmdID)
				{
					//������VIP�ͻ��˷������������������û�������Ϣ
					m_pPlayerMgr->SendMsg(*pack);
				}
				//	else if (s2c_begin_flying == cmdID)
				//	{
				//		//������VIP�ͻ��˷����������
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_begin_flying);
				//	}
				//	else if (s2c_play_video == cmdID)
				//	{
				//		//������VIP�ͻ��˷���������Ƶ����
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_play_video);
				//	}
				//	else if (s2c_stand_up == cmdID)
				//	{
				//		//������VIP�ͻ��˷���վ������
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_stand_up);
				//	}
				//	else if (s2c_walk == cmdID)
				//	{
				//		//������VIP�ͻ��˷�����ʼ��������
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_walk);
				//	}
				//	else if (s2c_client_list_external == cmdID)
				//	{
				//		//������VIP�ͻ��˷������ķ�����ת�����������������������ϵ��û��б�
				//		playerMgr->SendMsg(*pack);
				//	}
				//	else if (s2c_user_leave_external == cmdID)
				//	{
				//		//������VIP�ͻ��˷������ķ�����ת�����������������������ϵ��û��뿪��Ϣ
				//		playerMgr->SendMsg(*pack);
				//	}
				//	else if (s2s_req_usr_list == cmdID)
				//	{
				//		//�����û��糡���������ɼ�
				//		playerMgr->SetUserVisibilityExternal(true);

				//		playerMgr->SendClientListToCenterServer();
				//	}
					break;
				}
				//case ID_User_Login:
				//{
				//	int cmdID = pack->header()->id2;
				//	if (s2c_upd_user_state == cmdID)
				//	{
				//		//������VIP�ͻ��˷������ķ�����ת�����������������������ϵ��û�״̬�仯��Ϣ(��ʼ�׻��������׻��ȵ�)
				//		playerMgr->BroadcastExternalUserState(pack);
				//	}
				//	break;
				//}
			case ID_Global_Transform:
			{
				//
				m_pPlayerMgr->SendMsg(*pack);
				break;
			}
			} //switch end
		} // for end
	}// if end 	
}


PlayerManager*& VRSceneServer::GetPlayerManager()
{
	return m_pPlayerMgr;
}
