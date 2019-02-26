#include "NetClient.h"
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
	SceneControllerClient = CreateNetEvtClient();
	SceneControllerClient->SetEventCallback(OnConnectEvent);

	bConnectedToSceneController = false;

	g_pSceneController = this;
}

NetClient::~NetClient()
{
	if (SceneControllerClient)
	{
		delete SceneControllerClient;
		SceneControllerClient = NULL;
	}
}


int NetClient::ReadIniFile()
{

	GetModuleFileNameA(NULL, m_CfgFile, MAX_PATH);
	(strrchr(m_CfgFile, '\\'))[0] = 0; 
	strcat(m_CfgFile, "\\VRAgentConfig.ini");

	::GetPrivateProfileStringA("SceneController", "IP", "Error", m_SceneControllerIP, 16, m_CfgFile);
	::GetPrivateProfileStringA("SceneController", "port", "-1", m_SceneControllerPort, 6, m_CfgFile);

	m_SeatNumber = ::GetPrivateProfileIntA("VRAgentConfig", "SeatNmber", -1, m_CfgFile);

	return SUCCESS;

}


int NetClient::ConnectSceneController()
{

	if (SceneControllerClient->Connect(m_SceneControllerIP, m_SceneControllerPort) != 0)
	{
		LOG(error, "初始化与场景控制服务器的连接时出现错误!");
		return ERR_CONNECT_CENTER_SERVER;
	}


	LOG(info, "******正在开始与场景控制服务器连接......");
	SceneControllerClient->Start();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (bConnectedToSceneController)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}

}


//void NetClient::Run()
//{
//	while (true)
//	{
//
//		HandleNetEventFromSceneController();
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(1));
//	}
//}


void NetClient::OnConnectSceneController(int& msgID)
{
	if (link_connected == msgID)
	{
		LOG(info, "连接场景控制服务器成功！");
		bConnectedToSceneController = true;
	}
	else if (link_server_closed == msgID)
	{
		LOG(error, "场景控制服务器端断开了连接！");
		bConnectedToSceneController = false;
	}
	else if (link_server_failed == msgID)
	{
		LOG(info, "连接场景控制服务器失败！");
		bConnectedToSceneController = false;
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
					LOG(info, "*****场景控制器返回连接成功消息！");

					//向场景控制器上报座位号
					MessagePackage msgPackage;
					msgPackage.WriteHeader(ID_User_Login, c2s_tell_seat_num);
					msgPackage.WriteBody(&m_SeatNumber, sizeof(int));
					SceneControllerClient->Send(msgPackage);

					//重置物理机状态为不可回收状态
					::WritePrivateProfileStringA("Device", "RecycleFlag", "0", m_CfgFile);
				
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

						memset(m_UserID, 0, sizeof(m_UserID));
						memcpy(m_UserID, pack->body(), len);

						::WritePrivateProfileStringA("VRAgentConfig", "UserID", m_UserID, m_CfgFile);
						::WritePrivateProfileStringA("Device", "RecycleFlag", "0", m_CfgFile);

						LOG(info, "成功绑定了用户ID: %s !", m_UserID);
					}

					else if (s2c_device_status_changed == cmdID)
					{
						
						//char m_status[8] = { 0 };
						//int len = pack->GetBodyLength();
						//if (len > sizeof(m_status) - 1)
						//{
						//	len = sizeof(m_status) - 1;
						//}

						//memcpy(m_status, pack->body(), len);

						//::WritePrivateProfileStringA("VRAgentConfig", "Status", m_status, m_CfgFile);

						//LOG(info, "成功更新了机器状态: %s !", m_status);
					}


					break;
				}
			} //switch end
		} // for end

	}//if
}

void NetClient::HandleDeviceStatus()
{
	int recycleFlag = ::GetPrivateProfileIntA("Device", "RecycleFlag", 0, m_CfgFile);

	if (recycleFlag == 1)
	{
		//向场景控制器上报当前机器的状态是可回收状态
		MessagePackage msgPackage;
		msgPackage.WriteHeader(ID_VRClientAgent_Notify, c2s_device_status_changed);

		DeviceStatus devStatus;
		devStatus.seatNumber = m_SeatNumber;
		devStatus.status = recycleFlag;
		msgPackage.WriteBody(&devStatus, sizeof(DeviceStatus));

		SceneControllerClient->Send(msgPackage);
	}

}


void NetClient::Disconn()
{
	SceneControllerClient->Disconnect();
}

