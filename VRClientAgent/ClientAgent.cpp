#include "ClientAgent.h"
#include <thread>
#include <windows.h>

ClientAgent* g_pSceneController = NULL;

void __stdcall  ConnEveCallback(int& msgID)
{
	if (g_pSceneController)
	{
		return g_pSceneController->OnConnectHostController(msgID);
	}
}

ClientAgent::ClientAgent()
{
	m_pHostCtlrNetClient = CreateNetEvtClient();
	m_pHostCtlrNetClient->SetEventCallback(ConnEveCallback);

	m_bConnToHostCtlr = false;

	g_pSceneController = this;
}

ClientAgent::~ClientAgent()
{
	if (m_pHostCtlrNetClient)
	{
		delete m_pHostCtlrNetClient;
		m_pHostCtlrNetClient = NULL;
	}
}


int ClientAgent::ReadIniFile()
{

	GetModuleFileNameA(NULL, m_CfgFile, MAX_PATH_LEN);
	(strrchr(m_CfgFile, '\\'))[0] = 0; 
	strcat(m_CfgFile, "\\VRAgentConfig.ini");

	::GetPrivateProfileStringA("VRHostController", "IP", "Error", m_HostControllerIP, IP_ADDR_LENGTH, m_CfgFile);
	::GetPrivateProfileStringA("VRHostController", "port", "-1", m_HostControllerPort, IP_PORT_LENGTH, m_CfgFile);

	m_SeatNumber = ::GetPrivateProfileIntA("VRAgentConfig", "SeatNmber", -1, m_CfgFile);

	return SUCCESS;

}


int ClientAgent::ConnectHostController()
{

	if (m_pHostCtlrNetClient->Connect(m_HostControllerIP, m_HostControllerPort) != 0)
	{
		LOG(error, "初始化与VR主机控制服务器的连接时出现错误!");
		return ERR_CONNECT_CENTER_SERVER;
	}


	LOG(info, "******正在开始与VR主机控制服务器连接......");
	m_pHostCtlrNetClient->Start();

	std::this_thread::sleep_for(std::chrono::seconds(1));

	if (m_bConnToHostCtlr)
	{
		return SUCCESS;
	}
	else
	{
		return FAIL;
	}

}

void ClientAgent::OnConnectHostController(int& msgID)
{
	if (link_connected == msgID)
	{
		LOG(info, "连接VR主机控制服务器成功！");
		m_bConnToHostCtlr = true;
	}
	else if (link_server_closed == msgID)
	{
		LOG(error, "VR主机控制服务器端断开了连接！");
		m_bConnToHostCtlr = false;
	}
	else if (link_server_failed == msgID)
	{
		LOG(info, "连接VR主机控制服务器失败！");
		m_bConnToHostCtlr = false;
	}
}



void ClientAgent::HandleMessage()
{
	if (m_bConnToHostCtlr)
	{
		MsgQueue& msgQ = m_pHostCtlrNetClient->GetMsgQueue();
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
					LOG(info, "*****VR主机控制器返回连接成功消息！");

					//向VR主机控制器上报座位号
					MessagePackage msgPackage;
					msgPackage.WriteHeader(ID_User_Login, c2s_tell_seat_num);
					msgPackage.WriteBody(&m_SeatNumber, sizeof(int));
					m_pHostCtlrNetClient->Send(msgPackage);

					//重置物理机状态为不可回收状态
					::WritePrivateProfileStringA("Device", "RecycleFlag", "0", m_CfgFile);
				
					break;
				}
				case ID_HostCtlr_Notify:
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

void ClientAgent::HandleDeviceStatus()
{
	int recycleFlag = ::GetPrivateProfileIntA("Device", "RecycleFlag", 0, m_CfgFile);

	if (recycleFlag == 1)
	{
		//向VR主机控制器上报当前机器的状态是可回收状态
		MessagePackage msgPackage;
		msgPackage.WriteHeader(ID_VRClientAgent_Notify, c2s_device_status_changed);

		DeviceStatus devStatus;
		devStatus.seatNumber = m_SeatNumber;
		devStatus.status = recycleFlag;
		msgPackage.WriteBody(&devStatus, sizeof(DeviceStatus));

		m_pHostCtlrNetClient->Send(msgPackage);

		::WritePrivateProfileStringA("Device", "RecycleFlag", "2", m_CfgFile);
	}

}


void ClientAgent::Disconn()
{
	m_pHostCtlrNetClient->Disconnect();
}

