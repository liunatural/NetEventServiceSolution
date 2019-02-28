#include "VRSceneServer.h"
#include "PlayerManager.h"
#include <vector>
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
	confReader = NULL;
	pNetEventServer = NULL;
	playerMgr = NULL;

	bConnectCenterSvr = false;
	centerSvrClient = NULL;
}

VRSceneServer::~VRSceneServer()
{
	if (confReader)
	{
		delete confReader;
		confReader = NULL;
	}

	if (pNetEventServer)
	{
		delete pNetEventServer;
		pNetEventServer = NULL;
	}

	if (playerMgr)
	{
		delete playerMgr;
		playerMgr = NULL;
	}


}

int VRSceneServer::ReadConfigFile()
{
	confReader = CreateConfigReader();
	char path[MAX_PATH] = { 0 };
	_getcwd(path, MAX_PATH);
	strcat(path, "/VRSceneServer.xml");
	bool ret = confReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "�����ļ���ȡʧ��\n");
		return FAIL;
	}

}

int VRSceneServer::Start()
{
	
	if (!confReader)
	{
		LOG(error, "�����ļ�����confReaderΪNULL��");
		return FAIL;
	}
	
	int port = confReader->GetInt("root.Server.port");
	int maxlinks = confReader->GetInt("root.Server.maxlinks");
	confReader->GetStr("root.Server.name", sceneServerID);

	pNetEventServer = CreateNetEvtServer();

	bool bRet = pNetEventServer->Start(port, maxlinks);
	if (bRet == false)
	{
		return FAIL;
	}

	LOG(info, "���������������ɹ���");


	//pNetEventServer->Stop();
	//getchar();

	return SUCCESS;

}


int VRSceneServer::ConnectCenterSvr()
{
	char centerServerIP[50] = { 0 };
	char centerServerPort[50] = { 0 };
	confReader->GetStr("root.CenterServer.ip", centerServerIP);
	confReader->GetStr("root.CenterServer.port", centerServerPort);
	centerSvrClient = CreateNetEvtClient();
	centerSvrClient->SetEventCallback(OnConnectEvent);
	if (centerSvrClient->Connect(centerServerIP, centerServerPort) != 0)
	{
		LOG(error, "��ʼ�����ķ���������ʱ���ִ���!");
		return ERR_CONNECT_CENTER_SERVER;
	}

	LOG(info, "���ڿ�ʼ�����ķ���������......");
	centerSvrClient->Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(2));

	return SUCCESS;
}


int VRSceneServer::CreatePlayerManager()
{

	if (!pNetEventServer || strlen(sceneServerID) == 0)
	{
		LOG(error, "�����û�������ʧ�ܣ�");
		return FAIL;
	}

	playerMgr = new PlayerManager();

	playerMgr->SetNetworkService(pNetEventServer);
	playerMgr->SetSceneServerID(sceneServerID);

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
		bConnectCenterSvr = true;
		playerMgr->SetCenterSvrClient(centerSvrClient);
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

	MsgQueue& msgQ = pNetEventServer->GetMsgQueue();
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
			ply->SetSceneServerID(sceneServerID);
			playerMgr->AddPlayer(ply);

			playerMgr->SendCmd(cid, link_connected, 0, NULL, 0);

			playerMgr->SendUserInfoList(cid);

			//playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);

			break;
		}
		case  link_disconnected:
		{
			playerMgr->SendPlayerLeaveMsg(cid);

			break;
		}
		case ID_User_Login:
		{
			//if (c2s_tell_seat_num == cmdID)		//���մ�VIP�ͻ��˷��������κ���Ϣ
			//{
			//	int seatNum = *(int*)pack->body();

			//	//����λ�ŵ�һ�����
			//	bool bRet = playerMgr->UpdatePlayerSeatNumber(cid, seatNum);
			//	if (bRet)
			//	{
			//		//������VIP�ͻ��˹㲥��ǰ�û���״̬Ϊ��ʼ״̬
			//		playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);
			//	}
			//}
			//else if (c2s_tell_user_id == cmdID)		//
			//{
			//	char* userid = pack->body();
			//	int userid_len = pack->GetBodyLength();

			//	//��userid�ŵ�һ�����
			//	bool bRet = playerMgr->BindUserIDToPlayer(cid, userid, userid_len);
			//}

			if (c2s_tell_user_info == cmdID)
			{
				UserInfo *usrInfo = (UserInfo*)(pack->body());

				//�����û���Ϣ
				bool bRet = playerMgr->UpdateUserInfo(cid, usrInfo);
				if (bRet)
				{
					//������VIP�ͻ��˹㲥��ǰ�û���״̬Ϊ��ʼ״̬
					playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);
				}
			}


			break;
		}
		case ID_User_Transform:		//���λ�ñ任��Ϣ
		{
			TransformInfo* transInfo = (TransformInfo*)pack->body();
			transInfo->plyId = cid;

			playerMgr->UpdatePlayerTransform(cid, *transInfo);

			break;
		}
		case ID_Global_Notify:
		{

			if (cmdID == c2s_seen_external)
			{
				
				char* userid = pack->body();
				int userid_len = pack->GetBodyLength();
				
				bool bRet = playerMgr->UpdateUserTypeByUserID(userid, userid_len, ExternalVIP);
				if (bRet)
				{
					centerSvrClient->Send(ID_Global_Notify, s2c_trans_ext_usr_profile, (const char*)pack->body(), pack->GetBodyLength());
				}
			}
			else if(cmdID == c2s_stop_seen_external)
			{

				char* userid = pack->body();
				int userid_len = pack->GetBodyLength();

				bool bRet = playerMgr->UpdateUserTypeByUserID(userid, userid_len, VIP);
				if (bRet)
				{
					centerSvrClient->Send(ID_Global_Notify, s2c_trans_ext_usr_profile, (const char*)pack->body(), pack->GetBodyLength());
				}
			}
			break;
		}
		case ID_Global_Transform:
		{
			playerMgr->SendMsg(*pack);
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
	if (bConnectCenterSvr)
	{
		MsgQueue& msgQ = centerSvrClient->GetMsgQueue();
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
					playerMgr->SendMsg(*pack);
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
				playerMgr->SendMsg(*pack);
				break;
			}
			} //switch end
		} // for end
	}// if end 	

}


PlayerManager*& VRSceneServer::GetPlayerManager()
{
	return playerMgr;
}
