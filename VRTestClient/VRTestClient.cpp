// VRTestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetEventService.h"
#include "protocol.h"

#include <vector>
#include <thread>
#include <time.h>
#include <windows.h>

#define random(x) (rand()%x)

void Message_handle(void *args);

void Send_testPack(void *args);

int main()
{
	NetEvtClient* pNetEvClient = CreateNetEvtClient();


	if (!pNetEvClient)
	{
		return -1;
	}

	InitLogger("Log/Client");

	if (pNetEvClient->Connect("127.0.0.1", "5555") != 0)
	//if (pNetEvClient->Connect("192.168.2.48", "5555") != 0)
	{
		return -1;
	}

	std::thread send_thread(&Send_testPack, pNetEvClient);
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

			switch (msgID)
			{
			case link_stat::link_connected:
			{
				LOG(info, "Á¬½ÓOK£¡");
				break;
			}
			case ID_User_Notify:
			{
				int cmd_id = pack->header()->id2;
				if (0 == cmd_id)
				{
					int* j = (int*)pack->body();
					printf("recv from server : cnt = %d, msg = %d\n", count++, *j);
				}
				else if (1 == cmd_id)
				{
					printf("recv from server : cnt = %d, msg = %s\n", count++, pack->body());
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