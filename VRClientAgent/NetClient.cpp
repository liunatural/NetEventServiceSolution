#include "NetClient.h"
//#include <vector>
//#include <direct.h>
#include <thread>
#include <windows.h>

NetClient* g_pSceneController = NULL;

void __stdcall  OnConnectEvent(int& msgID)
{
	if (g_pSceneController)
	{
		return g_pSceneController->OnConnectSceneController(msgID);
	}
}

NetClient::NetClient()
{
	SceneControllerClient = NULL;
	bConnectedToSceneController = false;
}

NetClient::~NetClient()
{
}

int NetClient::ReadIniFile()
{
	::GetPrivateProfileStringA("SceneController", "IP", "Error", m_SceneControllerIP, 16, m_CfgFile);
	::GetPrivateProfileStringA("SceneController", "port", "-1", m_SceneControllerPort, 6, m_CfgFile);

	return SUCCESS;

}



int NetClient::ConnectSceneController()
{

	SceneControllerClient = CreateNetEvtClient();
	SceneControllerClient->SetEventCallback(OnConnectEvent);

	//��������
	int count = 0;
	while (count < 1200)
	{
		if (SceneControllerClient->Connect(m_SceneControllerIP, m_SceneControllerPort) != 0)
		{
			LOG(error, "��ʼ���볡�����Ʒ�����������ʱ���ִ���!");
			//return ERR_CONNECT_CENTER_SERVER;
		}

		LOG(info, "���ڿ�ʼ�볡�����Ʒ���������......");
		SceneControllerClient->Start();

		std::this_thread::sleep_for(std::chrono::seconds(3));

		if (bConnectedToSceneController)
		{
			return SUCCESS;
		}

		count++;
	}

	return FAIL;
}




void NetClient::Run()
{
	while (true)
	{

		HandleNetEventFromSceneController();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


void NetClient::OnConnectSceneController(int& msgID)
{
	if (link_connected == msgID)
	{
		LOG(info, "���ӳ������Ʒ������ɹ���");
		bConnectedToSceneController = true;
	}
	else if (link_server_closed == msgID)
	{
		LOG(error, "�������Ʒ������˶Ͽ������ӣ�");
	}
	else if (link_server_failed == msgID)
	{
		LOG(info, "���ӳ������Ʒ�����û�гɹ���");
	}
}



void NetClient::HandleNetEventFromSceneController()
{
	if (bConnectedToSceneController)
	{
		MsgQueue& msgQ = SceneControllerClient->GetMsgQueue();
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
				LOG(info, "���ӵ�����������OK��");

				MessagePackage msgPackage;
				int userType = VRClientAgent;
				msgPackage.WriteHeader(ID_User_Login, c2s_tell_user_type);
				msgPackage.WriteBody(&userType, sizeof(int));
				SceneControllerClient->Send(msgPackage);
				
				break;
			}
			case ID_SceneCntrl_Notify:
			{
				int cmdID = pack->header()->id2;
				if (s2c_tell_user_id == cmdID)
				{
					int len = pack->GetBodyLength();
					if (len > sizeof(m_UserID) -1 )
					{
						len = sizeof(m_UserID) - 1;
					}
					strncpy(m_UserID, pack->body(), len);
				}

				break;
			}
			} //switch end
		} // for end

	}//if
}

