// VRSceneServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NetEventService.h"
#include "protocol.h"
#include <vector>
#include <thread>
#include "windows.h"

void Message_handle(void *args);

std::vector<int> users;

int main()
{
	NetEvtServer* pNetEventServer = CreateNetEvtServer();

	if (!pNetEventServer)
	{
		return -1;
	}

	if (!pNetEventServer->Start(5555))
	{
		return -1;
	}


	Message_handle(pNetEventServer);

	delete pNetEventServer;

	getchar();

    return 0;
}

void Message_handle(void *args)
{
	NetEvtServer *pNetEventServer = (NetEvtServer *)args;

	int count = 0;
	while (true)
	{
		MsgQueue& msgQ = pNetEventServer->GetMsgQueue();
		int msgAmount = msgQ.GetCount();
		//printf(" message count : = %d \t##############\n", msgAmount);
		for (int i = 0; i < msgAmount; i++)
		{
			MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

			int msgID = pack->header()->id1;

			switch (msgID)
			{
			case link_stat::link_connected:
			{
				int cid = pack->GetLinkID();
				printf("%04d 用户上线！\n", cid);
				users.push_back(cid);

				pNetEventServer->Send(cid, *pack);

				break;
			}
			case  link_stat::link_disconnected:
			{
				int cid = pack->GetLinkID();
				char ip[16] = { 0 };
				memcpy(ip, pack->body(), pack->GetBodyLength());
				printf("[%s]下线！\n", ip);

				for (std::vector<int>::iterator it = users.begin(); it != users.end(); it++)
				{
					int id = *it;
					if (id == cid)
					{
						users.erase(it);
						break;
					}
				}
				break;
			}
			default:
			{
				//int* j = (int*)pack->body();
				//printf("recv : cnt = %d, tid = %d, msg = %d\n", count++, pack->header()->id1, *j);

				std::vector<int>::iterator it = users.begin();
				while (it != users.end())
				{
					int targetid = *it;
					if (targetid >= 0)
					{
						int sourid = pack->GetLinkID();
		
						if (sourid != targetid)
						{
							pNetEventServer->Send(targetid, *pack);
						}
					}

					++it;
				}
				break;
			}

			}


		}//for

		Sleep(1);

	}//while
}