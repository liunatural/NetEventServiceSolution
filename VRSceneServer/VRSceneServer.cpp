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
		LOG(error, "配置文件读取失败\n");
		return FAIL;
	}

}

int VRSceneServer::Start()
{
	
	if (!confReader)
	{
		LOG(error, "配置文件对象confReader为NULL！");
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

	LOG(info, "场景服务器启动成功！");


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
		LOG(error, "初始化中心服务器连接时出现错误!");
		return ERR_CONNECT_CENTER_SERVER;
	}

	LOG(info, "正在开始与中心服务器连接......");
	centerSvrClient->Start();

	std::this_thread::sleep_for(std::chrono::milliseconds(2));

	return SUCCESS;
}


int VRSceneServer::CreatePlayerManager()
{

	if (!pNetEventServer || strlen(sceneServerID) == 0)
	{
		LOG(error, "创建用户管理器失败！");
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
		LOG(info, "连接中心服务器成功！");
		bConnectCenterSvr = true;
		playerMgr->SetCenterSvrClient(centerSvrClient);
	}
	else if (link_server_closed == msgID)
	{
		LOG(error, "中心服务器端断开了连接！");
	}
	else if (link_server_failed == msgID)
	{
		LOG(info, "连接中心服务器没有成功！");
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
			//if (c2s_tell_seat_num == cmdID)		//接收从VIP客户端发来的座椅号消息
			//{
			//	int seatNum = *(int*)pack->body();

			//	//绑定座位号到一个玩家
			//	bool bRet = playerMgr->UpdatePlayerSeatNumber(cid, seatNum);
			//	if (bRet)
			//	{
			//		//向其他VIP客户端广播当前用户的状态为初始状态
			//		playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);
			//	}
			//}
			//else if (c2s_tell_user_id == cmdID)		//
			//{
			//	char* userid = pack->body();
			//	int userid_len = pack->GetBodyLength();

			//	//绑定userid号到一个玩家
			//	bool bRet = playerMgr->BindUserIDToPlayer(cid, userid, userid_len);
			//}

			if (c2s_tell_user_info == cmdID)
			{
				UserInfo *usrInfo = (UserInfo*)(pack->body());

				//更新用户信息
				bool bRet = playerMgr->UpdateUserInfo(cid, usrInfo);
				if (bRet)
				{
					//向其他VIP客户端广播当前用户的状态为初始状态
					playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);
				}
			}


			break;
		}
		case ID_User_Transform:		//玩家位置变换消息
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
					//向所有VIP客户端发送其他场景服务器用户描述信息
					playerMgr->SendMsg(*pack);
				}
				//	else if (s2c_begin_flying == cmdID)
				//	{
				//		//向所有VIP客户端发出起飞命令
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_begin_flying);
				//	}
				//	else if (s2c_play_video == cmdID)
				//	{
				//		//向所有VIP客户端发出播放视频命令
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_play_video);
				//	}
				//	else if (s2c_stand_up == cmdID)
				//	{
				//		//向所有VIP客户端发出站立命令
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_stand_up);
				//	}
				//	else if (s2c_walk == cmdID)
				//	{
				//		//向所有VIP客户端发出开始行走命令
				//		playerMgr->BroadcastControlCmd(ID_User_control, s2c_walk);
				//	}
				//	else if (s2c_client_list_external == cmdID)
				//	{
				//		//向所有VIP客户端发送中心服务器转发过来的其它场景服务器上的用户列表
				//		playerMgr->SendMsg(*pack);
				//	}
				//	else if (s2c_user_leave_external == cmdID)
				//	{
				//		//向所有VIP客户端发送中心服务器转发过来的其它场景服务器上的用户离开消息
				//		playerMgr->SendMsg(*pack);
				//	}
				//	else if (s2s_req_usr_list == cmdID)
				//	{
				//		//设置用户跨场景服务器可见
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
				//		//向所有VIP客户端发送中心服务器转发过来的其它场景服务器上的用户状态变化消息(开始献花，结束献花等等)
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
