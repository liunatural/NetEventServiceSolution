#include "VRSceneServer.h"
#include <vector>
#include <thread>

#include <direct.h>
#include <windows.h>
#include "PlayerManager.h"


VRSceneServer::VRSceneServer()
{
	confReader = NULL;
	pNetEventServer = NULL;
	playerMgr = NULL;
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
	strcat(path, "/SceneService.xml");
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
	if (!pNetEventServer->Start(port))
	{
		return FAIL;
	}
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

void VRSceneServer::HandleNetEvent()
{
	int count = 0;
	while (true)
	{
		MsgQueue& msgQ = pNetEventServer->GetMsgQueue();
		int msgAmount = msgQ.GetCount();

		for (int i = 0; i < msgAmount; i++)
		{
			MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

			int msgID = pack->header()->id1;
			int cid = pack->GetLinkID();

			switch (msgID)
			{
			case link_connected:
			{

				Player*  ply = new Player(cid);
				ply->SetSeatNumber(cid);
				ply->SetSceneServerID(sceneServerID);
				playerMgr->AddPlayer(ply);

				playerMgr->SendClientList(cid);

				playerMgr->BroadcastUserState(cid, ID_User_Login, state_initial);

				break;
			}
			case  link_disconnected:
			{
				playerMgr->SendPlayerLeaveMsg(cid);
				break;
			}
			case ID_User_Transform:		//玩家位置变换消息
			{
				TransformInfo* transInfo = (TransformInfo*)pack->body();
				transInfo->plyId = cid;

				playerMgr->UpdatePlayerTransform(cid, *transInfo);

				break;
			}
			default:
			{
				////int* j = (int*)pack->body();
				////printf("recv : cnt = %d, tid = %d, msg = %d\n", count++, pack->header()->id1, *j);

				//std::vector<int>::iterator it = users.begin();
				//while (it != users.end())
				//{
				//	int targetid = *it;
				//	if (targetid >= 0)
				//	{
				//		int sourid = pack->GetLinkID();

				//		if (sourid != targetid)
				//		{
				//			pNetEventServer->Send(targetid, *pack);
				//		}
				//	}

				//	++it;
				//}
				break;
			}

			}


		}//for

		Sleep(1);

	}//while
}

PlayerManager*& VRSceneServer::GetPlayerManager()
{
	return playerMgr;
}
