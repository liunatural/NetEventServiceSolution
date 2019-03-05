//**************************************************************************
//  File......... : VRHostController.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Implementation file of the class VRHostController used as business logics process
//							of VR Host Controller server.
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
//***************************************************************************
#include <direct.h>
#include <thread>

#ifdef _WINDOWS
#include  "../VRHostControllerUI/VRHostControllerUIDlg.h"
#endif

#include "VRHostController.h"
#include "RemoteClientManager.h"
#include "RemoteClient.h"
#include "Util.h"

VRHostController::VRHostController()
{
	m_pConfReader = NULL;
	m_pNetEventServer = NULL;
	m_pClientMgr = NULL;

	m_pCSVFile = NULL;

	bStopFlag = false;

}

VRHostController::VRHostController(CVRHostControllerUIDlg *pUIDlg)
{
	m_pVRHostControllerUIDlg = pUIDlg;
	
	m_pConfReader = NULL;
	m_pNetEventServer = NULL;
	m_pClientMgr = NULL;

	m_pCSVFile = NULL;

	bStopFlag = false;
}

VRHostController::~VRHostController()
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

	if (m_pClientMgr)
	{
		delete m_pClientMgr;
		m_pClientMgr = NULL;
	}

	if (m_pCSVFile)
	{
		delete m_pCSVFile;
		m_pCSVFile = NULL;
	}

}

int VRHostController::ReadConfigFile()
{
	m_pConfReader = CreateConfigReader();
	char path[MAX_PATH] = { 0 };
	_getcwd(path, MAX_PATH);
	strcat(path, "/VRHostController.xml");
	bool ret = m_pConfReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "配置文件读取失败\n");
		return FAIL;
	}

}

int VRHostController::Start()
{
	
	if (!m_pConfReader)
	{
		LOG(error, "配置文件对象confReader为NULL！");
		return FAIL;
	}
	
	m_port = m_pConfReader->GetInt("root.Server.port");
	m_maxLinks = m_pConfReader->GetInt("root.Server.maxlinks");
	m_pConfReader->GetStr("root.Server.name", m_HostCtlrID);

	m_pNetEventServer = CreateNetEvtServer();

	bool bRet = m_pNetEventServer->Start(m_port, m_maxLinks);
	if (bRet == false)
	{
		return FAIL;
	}

	LOG(info, "VR主机控制器启动成功！");


	return SUCCESS;

}

int VRHostController::CreateVRClientManager()
{

	if (!m_pNetEventServer || strlen(m_HostCtlrID) == 0)
	{
		LOG(error, "创建用户管理器失败！");
		return FAIL;
	}

	m_pClientMgr = new RemoteClientManager();

	m_pClientMgr->SetNetworkService(m_pNetEventServer);
	m_pClientMgr->SetHostController(this);

	return SUCCESS;

}


void VRHostController::Run()
{
	while (true)
	{
		if (bStopFlag == true)
		{
			break;
		}
		
		HandleNetEventFromClient();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}


bool VRHostController::Stop()
{
	return m_pNetEventServer->Stop();
}

int VRHostController::CreateUserSeatMap()
{
	if (!m_pCSVFile)
	{
		m_pCSVFile = new CSVFile();
	}

	m_pCSVFile->OpenFile();
	int ret =  m_pCSVFile->ReadUserSeatMap();
	if (ret != 0)
	{
		LOG(error, "读取user_seat_map失败！");
	}
	else
	{
		m_USM = m_pCSVFile->GetUserSeatMap();
	}

	//char* userid;
	//int seat;

	//User_Seat_Map::iterator it = m_USM.begin();
	//while (it != m_USM.end())
	//{
	//	seat = it->first;
	//	userid =(char*) (it->second).c_str();

	//	cout << (*it).first << (*it).second << endl;

	//	it++;
	//}

	//m_pCSVFile->Write(6, "wefrewrwerwe023");

	return ret;
}

void VRHostController::HandleNetEventFromClient()
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
				RemoteClient*  pClient = new RemoteClient(cid);
				pClient->SaveIP(pack->body(), pack->GetBodyLength());

				m_pClientMgr->AddRemoteClient(pClient);
				m_pClientMgr->SendCmd(cid, link_connected, 0, NULL, 0);

				SendLogMsg(m_pClientMgr->GetOutputLog());
				
				break;
			}
			case  link_disconnected:
			{
				m_pClientMgr->DeleteRemoteClient(cid);

				break;
			}
			case ID_User_Login:
			{
				if (c2s_tell_seat_num == cmdID)		//接收从VRClientAgent发来的座椅号消息
				{
					int seatNum = *(int*)pack->body();

					bool bRet = m_pClientMgr->AssignSeatNumToVRHost(cid, seatNum);
				}
				else if (c2s_tell_user_id == cmdID)		//处理胶囊体发来的消息
				{

					//更新终端类型为胶囊体类型, 一定要先更新类型，后绑定
					m_pClientMgr->UpdateClientType(cid, Capsule);

					int len_userID = pack->GetBodyLength();
					CopyData(m_UserID, pack->body(), len_userID, USER_ID_LENGTH);

					RemoteClient *client = NULL;
					bool bRet = m_pClientMgr->AllocateVRHostForUser(m_UserID, len_userID, &client);
					if (bRet)
					{
						int seatNumber = client->GetSeatNumber();
						UserInfo usrInfo;

						memcpy(usrInfo.UserID, m_UserID, len_userID);
						usrInfo.SeatNumber = seatNumber;

						MessagePackage package;
						package.WriteHeader(ID_SceneCntrl_Notify, s2c_rsp_seat_num);
						package.WriteBody(&usrInfo, sizeof(UserInfo));

						//返回座席号给胶囊体
						m_pClientMgr->SendMsg(cid, package);

						//向终端代理发送绑定的userID
						MessagePackage package1;
						package1.WriteHeader(ID_SceneCntrl_Notify, s2c_tell_user_id);
						package1.WriteBody(m_UserID, len_userID);
						m_pClientMgr->SendMsg(client->GetLinkID(), package1);

						//向csv文件写入SeatNumber-UserID映射关系
						m_pCSVFile->Write(seatNumber, usrInfo.UserID, client->GetIP());
					}
					else
					{
						UserInfo usrInfo;

						memcpy(usrInfo.UserID, m_UserID, len_userID);
						usrInfo.SeatNumber = -1;

						MessagePackage package;
						package.WriteHeader(ID_SceneCntrl_Notify, s2c_rsp_seat_num);
						package.WriteBody(&usrInfo, sizeof(UserInfo));
					
						//返回座席号-1给胶囊体
						m_pClientMgr->SendMsg(cid, package);

					}
				}

				break;
			}
			case ID_VRClientAgent_Notify:
			{
				if (c2s_device_status_changed == cmdID)
				{
					DeviceStatus  *pDevStatus = (DeviceStatus*)pack->body();
				
					int seatNum = pDevStatus->seatNumber;
					int stats = pDevStatus->status;

					if (DeviceState::RecycleEnable == stats)
					{
						m_pClientMgr->ReclaimVRHost(cid, seatNum);
					}
				}
			}
			default:
			{
				break;
			}

		} //switch
	}//for

}


void VRHostController::SendLogMsg(Output_Log& outLog)
{

	LOG(outLog.m_logType, outLog.m_logStr.c_str());

#ifdef _WINDOWS
	m_pVRHostControllerUIDlg->SendUserConnectMsg(&outLog);
#endif
}



