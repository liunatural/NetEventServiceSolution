#include "VRSceneController.h"
#include "VRClientManager.h"
#include <direct.h>
#include <thread>

VRSceneController::VRSceneController()
{
	confReader = NULL;
	pNetEventServer = NULL;
	clientMgr = NULL;

}

VRSceneController::~VRSceneController()
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

	if (clientMgr)
	{
		delete clientMgr;
		clientMgr = NULL;
	}
}

int VRSceneController::ReadConfigFile()
{
	confReader = CreateConfigReader();
	char path[MAX_PATH] = { 0 };
	_getcwd(path, MAX_PATH);
	strcat(path, "/SceneController.xml");
	bool ret = confReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "�����ļ���ȡʧ��\n");
		return FAIL;
	}

}

int VRSceneController::Start()
{
	
	if (!confReader)
	{
		LOG(error, "�����ļ�����confReaderΪNULL��");
		return FAIL;
	}
	
	int port = confReader->GetInt("root.Server.port");
	int maxlinks = confReader->GetInt("root.Server.maxlinks");
	confReader->GetStr("root.Server.name", sceneControllerID);

	pNetEventServer = CreateNetEvtServer();

	bool bRet = pNetEventServer->Start(port, maxlinks);
	if (bRet == false)
	{
		return FAIL;
	}

	LOG(info, "���������������ɹ���");


	return SUCCESS;

}

int VRSceneController::CreateVRClientManager()
{

	if (!pNetEventServer || strlen(sceneControllerID) == 0)
	{
		LOG(error, "�����û�������ʧ�ܣ�");
		return FAIL;
	}

	clientMgr = new VRClientManager();

	clientMgr->SetNetworkService(pNetEventServer);

	return SUCCESS;

}


void VRSceneController::Run()
{
	while (true)
	{
		HandleNetEventFromClient();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void VRSceneController::HandleNetEventFromClient()
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

			VRClient*  pClient = new VRClient(cid);
			clientMgr->AddVRClient(pClient);

			clientMgr->SendCmd(cid, link_connected, 0, NULL, 0);

			break;
		}
		case  link_disconnected:
		{
			clientMgr->DeleteVRClientFromList(cid);

			break;
		}
		case ID_User_Login:
		{
			if (c2s_tell_seat_num == cmdID)		//���մ�VRClientAgent���������κ���Ϣ
			{
				int seatNum = *(int*)pack->body();

				bool bRet = clientMgr->UpdateSeatNumber(cid, seatNum);

			}
			else if (c2s_tell_user_id == cmdID)		//�������巢������Ϣ
			{
				char* userid = pack->body();
				int userid_len = pack->GetBodyLength();
				int seatNumber = -1;

				//��userid�ŵ�һ��VR�ն�
				bool bRet = clientMgr->BindUserIDToVRClient(userid, userid_len, seatNumber);

				if (bRet)
				{
					//�����ն�����Ϊ����������
					clientMgr->UpdateClientType(cid, Capsule);

					UserInfo usrInfo;
					memcpy(usrInfo.UserID, userid, userid_len);
					usrInfo.SeatNumber = seatNumber;

					MessagePackage package;
					package.WriteHeader(ID_SceneCntrl_Notify, s2c_rsp_seat_num);
					package.WriteBody(&usrInfo, sizeof(UserInfo));

					//������ϯ�Ÿ�������
					clientMgr->SendMsg(cid, package);
				}

			}

			break;
		}
		default:
		{
			break;
		}

		} //switch
	}//for

}
