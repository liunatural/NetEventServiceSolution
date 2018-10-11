// VRTestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetEventService.h"
#include "protocol.h"

#include <vector>
#include <thread>
#include <time.h>
#include <windows.h>
#include "direct.h"

#define random(x) (rand()%x)

void Message_handle(void *args);

void Send_testPack(void *args);

void Send_TransformPack(void *args);


int main()
{
	InitLogger("Log/Client");

	NetEvtClient* pNetEvClient = CreateNetEvtClient();
	if (!pNetEvClient)
	{
		return -1;
	}

	ConfigService* confReader = CreateConfigReader();
	char path[MAX_PATH] = { 0 };
	_getcwd(path, MAX_PATH);
	strcat(path, "/ClientConfig.xml");
	bool ret = confReader->OpenFile(path);
	if (!ret)
	{
		LOG(error, "打开配置文件失败！\n");
		return -1;
	}


	char port[100] = { 0 };
	char ip[100] = { 0 };
	confReader->GetStr("root.Server.port", port);
	confReader->GetStr("root.Server.ip", ip);

	if (port == 0 || ip == 0)
	{
		LOG(error, "从配置文件中读取IP地址或端口号出错！");
		return -1;
	}


	if (pNetEvClient->Connect(ip, port) != 0)
	{
		return -1;
	}

	std::thread send_thread(&Send_TransformPack, pNetEvClient);
	//std::thread send_thread(&Send_testPack, pNetEvClient);
	send_thread.detach();

	pNetEvClient->Start();

	Message_handle(pNetEvClient);
}

void Message_handle(void *args)
{
	NetEvtClient *pNetEventClient = (NetEvtClient *)args;

	int count = 0;

	while (true)
	{
		MsgQueue& msgQ = pNetEventClient->GetMsgQueue();
		int msgAmount = msgQ.GetCount();
		//printf(" message amount : = %d \t##############\n", msgAmount);
		for (int i = 0; i < msgAmount; i++)
		{
			MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

			int msgID = pack->header()->id1;
			int cmd_id = pack->header()->id2;

			switch (msgID)
			{
			case link_stat::link_connected:
			{
				LOG(info, "连接OK！");
				break;
			}
			case ID_User_Transform:
			{
				const char * p = pack->body();
				int s = pack->header()->length;

				if (s > 0)
				{
					int cout = s / sizeof(TransformInfo);

					int v = 0;
					while (v < cout)
					{
						TransformInfo* transInfo = (TransformInfo*)p;
						char buffer[100] = { 0 };
						printf("%04d用户发送位置变换信息[%4.2f_%4.2f_%4.2f]\n", transInfo->plyId, transInfo->pos.x, transInfo->pos.y, transInfo->pos.z);

						p += sizeof(TransformInfo);

						v++;
					}
				}
				break;
			}
			case ID_User_Login:
			{

				if (s2c_upd_user_state == cmd_id)
				{
					UserStateInfo usrStatInfo = *(UserStateInfo*)(pack->body());
					printf("新用户:%d，当前状态:%d\n", usrStatInfo.seatNumber, usrStatInfo.userState);
				}

				break;
			}
			case ID_User_Notify:
			{

				if (0 == cmd_id)
				{
					int* j = (int*)pack->body();
					printf("recv from server : cnt = %d, msg = %d\n", count++, *j);
				}
				else if (1 == cmd_id)
				{
					printf("recv from server : cnt = %d, msg = %s\n", count++, pack->body());
				}
				else if (s2c_client_list == cmd_id) //用户列表
				{

					const char * p = pack->body();
					int s = pack->GetBodyLength();

					if (s > 0)
					{
						int cout = s / sizeof(ProfileInfo);

						int v = 0;
						printf("+++++++++++++++++++++++++++++++++++++++++++++++++\r\n");
						while (v < cout)
						{
							ProfileInfo* profileInfo = (ProfileInfo*)p;
							
							printf("%04d座椅开机\r\n", profileInfo->mSeatNumber);

							p += sizeof(ProfileInfo);

							v++;
						}
						printf("///////////////////////////////////////////\r\n");
					}
				}
				else if (s2c_ply_leave == cmd_id)  //用户下线
				{
					int cid = *(int *)(pack->body());
					printf("%04d用户下线\n", cid);
				}

				break;
			}
			default:
			{
				break;
			}

			}

		}//for

		Sleep(1);

	}//while
}

void Send_testPack(void *args)
{
	NetEvtClient *pNetEventClient = (NetEvtClient *)args;

	time_t t;
	srand((unsigned)time(&t));

	for (int n = 0; n < 10000; n++)
	{

		int id = random(30) + 1;

		MessagePackage msgPackage;
		msgPackage.header()->id1 = (unsigned short)ID_User_Notify;
		msgPackage.header()->id2 = 0;
		memcpy(msgPackage.body(), &id, sizeof(int));
		msgPackage.SetBodyLength(sizeof(int));

		pNetEventClient->Send(msgPackage);

		Sleep(10);
	}

	for (int n = 0; n < 10000; n++)
	{

		int id = random(30) + 1;

		MessagePackage msgPackage;
		msgPackage.header()->id1 = (unsigned short)ID_User_Notify;
		msgPackage.header()->id2 = 1;
		memcpy(msgPackage.body(), "sdfsdfdsfd", sizeof("sdfsdfdsfd"));
		msgPackage.SetBodyLength(sizeof("sdfsdfdsfd"));

		pNetEventClient->Send(msgPackage);

		Sleep(10);
	}
}



void Send_TransformPack(void *args)
{
	NetEvtClient *pNetEventClient = (NetEvtClient *)args;
	//发送位置变换信息
	LOG(info, "发送位置变换信息\n");
	int x = 1;
	int y = 100;
	for (int count = 0; count < 10000; count++)
	{
		vec3 pos = { ++x, ++x, ++x };
		vec3 dir = { ++y, ++y, ++y };
		TransformInfo TransformInfo;
		TransformInfo.pos = pos;
		TransformInfo.dir = dir;
		int len = sizeof(TransformInfo);
		MessagePackage msgPackage;
		msgPackage.header()->id1 = ID_User_Transform; //发送位置变换信息
		msgPackage.header()->id2 = 0;
		msgPackage.header()->length = len;
		memcpy(msgPackage.body(), (void*)&TransformInfo, len);

		pNetEventClient->Send(msgPackage);

		Sleep(10);
	}

}
