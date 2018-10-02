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

	if (pNetEvClient->Connect("127.0.0.1", "5555") != 0)
	{
		printf("连接服务器失败！\n");
		return -1;
	}

	std::thread work_thread(&Message_handle, pNetEvClient);
	work_thread.detach();

	std::thread send_thread(&Send_testPack, pNetEvClient);
	send_thread.detach();

	pNetEvClient->Start();

	return 0;
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
				printf("连接OK！\n");
				break;
			}
			case ID_User_Notify:
			{
				int* j = (int*)pack->body();
				printf("recv from server : cnt = %d, msg = %d\n", count++, *j);

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

		Sleep(30);
	}



}