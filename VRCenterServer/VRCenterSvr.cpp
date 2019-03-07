#include "VRCenterSvr.h"
#include <vector>
#include <direct.h>
#include <windows.h>
#include "UserManager.h"



VRCenterSvr::VRCenterSvr()
{
	confReader = NULL;
	pNetEventServer = NULL;
	userMgr = NULL;
}


VRCenterSvr::~VRCenterSvr()
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

	if (userMgr)
	{
		delete userMgr;
		userMgr = NULL;
	}
}

int VRCenterSvr::ReadConfigFile()
{
	confReader = CreateConfigReader();
	char path[MAX_PATH_LEN] = { 0 };
	_getcwd(path, MAX_PATH_LEN);
	strcat(path, "/VRCenterServer.xml");
	bool ret = confReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "�����ļ���ȡʧ��");
		return FAIL;
	}
}

int VRCenterSvr::Start()
{
	if (!confReader)
	{
		LOG(error, "�����ļ�����confReaderΪNULL��");
		return FAIL;
	}

	int port = confReader->GetInt("root.Server.port");
	int maxlinks = confReader->GetInt("root.Server.maxlinks");

	pNetEventServer = CreateNetEvtServer();

	bool bRet = pNetEventServer->Start(port, maxlinks);
	if (bRet == false)
	{
		return FAIL;
	}

	LOG(info, "���ķ����������ɹ���");
	return SUCCESS;
}

int VRCenterSvr::CreateUserManager()
{
	if (!pNetEventServer)
	{
		LOG(error, "�����û�������ʧ�ܣ�");
		return FAIL;
	}

	userMgr = new UserManager();

	userMgr->SetNetworkService(&pNetEventServer);

	return SUCCESS;
}

void VRCenterSvr::HandleNetEvent()
{
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
				User*  user = new User(cid);
				userMgr->AddUser(user);
				break;
			}
			case link_disconnected:
			{
				userMgr->DeleteUser(cid);
				break;
			}
			case ID_User_Login:
			{
				int cmdID = pack->header()->id2;
				if (s2c_upd_user_state == cmdID)
				{
					//����������������ת���û�״̬�仯��Ϣ��Ŀǰ��ʼ�׻��������׻�״̬��
					userMgr->ForwardMsgToOtherSceneServers(cid, pack);
				}
				break;
			}
			case ID_Global_Notify:
			{
				int cmdID = pack->header()->id2;

				if (s2c_trans_ext_usr_profile == cmdID)
				{
					//����������������ת���ⲿ�û���������
					userMgr->ForwardMsgToOtherSceneServers(cid, pack);
				}
				//else if (s2c_begin_flying == cmdID)
				//{
				//	//�����г���������ת���������
				//	userMgr->ForwardCommand(pack);
				//}
				//else if (s2c_play_video == cmdID)
				//{
				//	//�����г���������ת��������Ƶ����
				//	userMgr->ForwardCommand(pack);
				//}
				//else if (s2c_stand_up == cmdID)
				//{
				//	//�����г���������ת��վ������
				//	userMgr->ForwardCommand(pack);
				//}
				//else if (s2c_walk == cmdID)
				//{
				//	//�����г���������ת����������
				//	userMgr->ForwardCommand(pack);
				//}
				//else if (s2c_client_list_external == cmdID)
				//{
				//	//����������������ת����һ�������������û��б�
				//	userMgr->ForwardMsg(cid, pack);
				//}
				//else if (s2c_user_leave_external == cmdID)
				//{
				//	//����������������ת����һ�������������û��뿪��Ϣ
				//	userMgr->ForwardMsg(cid, pack);
				//}
				//else if (s2s_req_usr_list == cmdID)
				//{
				//	userMgr->ForwardMsg(cid, pack);
				//}

				break;
			}
			case ID_Global_Transform:
			{
				//����������������ת����һ�������������û��˶��任��Ϣ
				userMgr->ForwardMsgToOtherSceneServers(cid, pack);
				break;
			}

			case ID_User_Transform:
			{
				TransformInfo* transInfo = (TransformInfo*)pack->body();
				transInfo->plyId = cid;
				//userMgr->UpdatePlayerTransform(cid, *transInfo);
				break;
			}
		
			}// switch end
		}// for loop end

		Sleep(10);

	} //while end
}

UserManager*& VRCenterSvr::GetUserManager()
{
	return userMgr;
}
