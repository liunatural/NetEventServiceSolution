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
void Send_SeatNumber(void *args);
void Send_Seen_External(void *args);
void Send_Tell_UserID(void *args);
void Send_UserInfo(void *args);

std::thread send_thread;

int seatNum;

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

		for (int i = 0; i < msgAmount; i++)
		{
			MessagePackage* pack = (MessagePackage*)msgQ.GetMsg(i);

			int msgID = pack->header()->id1;
			int cmd_id = pack->header()->id2;

			switch (msgID)
			{
			case link_connected:
			{
				LOG(info, "连接OK！");

				//Send_SeatNumber(pNetEventClient);

				
				//Send_Tell_UserID(pNetEventClient);

				Send_UserInfo(pNetEventClient);

				//Sleep(10);
				//Send_Seen_External(pNetEventClient);
	
				//send_thread = std::thread(&Send_testPack, pNetEventClient);
	
				//Sleep(10);
				//send_thread = std::thread(&Send_TransformPack, pNetEventClient);



				break;
			}
			case link_error_exceed_max_connects:
			{
				LOG(info, "超过服务器最大连接数！请稍后重试！");
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
						LOG(info, "%04d用户,座椅号[%d]:发送位置变换信息[%4.2f_%4.2f_%4.2f]", 
							transInfo->plyId, transInfo->seatNumber, transInfo->pos.x, transInfo->pos.y, transInfo->pos.z);

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
					LOG(info, "新用户上线： 座椅号[%d]，当前状态:%d", usrStatInfo.seatNumber, usrStatInfo.userState);
				}

				break;
			}
			case ID_User_Notify:
			{
				if (s2c_client_list == cmd_id) //用户列表
				{

					const char * p = pack->body();
					int s = pack->GetBodyLength();

					if (s > 0)
					{
						int cout = s / sizeof(ProfileInfo);

						int v = 0;
						LOG(info, "///////////////////////////////////////////");
						LOG(info, "当前在线用户：");
						while (v < cout)
						{
							ProfileInfo* profileInfo = (ProfileInfo*)p;
							
							LOG(info, "座椅[%04d]", profileInfo->mSeatNumber);

							p += sizeof(ProfileInfo);

							v++;
						}
						LOG(info, "///////////////////////////////////////////");
					}
				}
				else if (s2c_ply_leave == cmd_id)  //用户下线
				{
					int cid = *(int *)(pack->body());
					LOG(info, "%04d用户下线", cid);
				}

				break;
			}
			case ID_Global_Notify:
			{
				int cmdID = pack->header()->id2;
				if (s2c_trans_ext_usr_profile == cmdID)
				{
					ProfileInfo *pProfInfo = (ProfileInfo*)pack->body();
					LOG(info, "外部用户座椅号：%d", pProfInfo->mSeatNumber);
				}
				break;
			}
			case ID_Global_Transform:
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
						sprintf(buffer, "外部用户%04d发送位置变换信息[%4.2f_%4.2f_%4.2f]\n", transInfo->plyId, transInfo->pos.x, transInfo->pos.y, transInfo->pos.z);
						printf(buffer);

						p += sizeof(TransformInfo);

						v++;
					}
				}
				break;
			}

			default:
			{
				break;
			}

			}

		}//for

		Sleep(10);

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

		msgPackage.WriteHeader(ID_User_Notify, 0);
		msgPackage.WriteBody(&id, sizeof(int));


		pNetEventClient->Send(msgPackage);

		Sleep(10);
	}

	for (int n = 0; n < 10000; n++)
	{

		int id = random(30) + 1;

		MessagePackage msgPackage;

		msgPackage.WriteHeader(ID_User_Notify, 0);
		msgPackage.WriteBody("sdfsdfdsfd", sizeof("sdfsdfdsfd"));


		pNetEventClient->Send(msgPackage);

		Sleep(10);
	}
}



void Send_TransformPack(void *args)
{
	NetEvtClient *pNetEventClient = (NetEvtClient *)args;

	LOG(info, "发送位置变换信息\n");
	int x = 1;
	int y = 100;
	for (int count = 0; count < 10000; count++)
	{
		vec3 pos = { ++x, ++x, ++x };
		vec3 dir = { ++y, ++y, ++y };
		TransformInfo transformInfo;
		transformInfo.seatNumber = seatNum;
		transformInfo.pos = pos;
		transformInfo.dir = dir;
		
		int len = sizeof(TransformInfo);
		MessagePackage msgPackage;

		msgPackage.WriteHeader(ID_User_Transform, 0);
		msgPackage.WriteBody(&transformInfo, len);


		pNetEventClient->Send(msgPackage);

		Sleep(5);
	}

}



void Send_SeatNumber(void *args)
{

	time_t t;
	srand((unsigned)time(&t));

	NetEvtClient *pNetEventClient = (NetEvtClient *)args;
	seatNum = random(10) + 1;
	int len = sizeof(int);
	MessagePackage msgPackage;
	msgPackage.WriteHeader(ID_User_Login, c2s_tell_seat_num);
	msgPackage.WriteBody(&seatNum, len);
	pNetEventClient->Send(msgPackage);

	LOG(info, "向服务器发送座椅号:[%d]\n", seatNum);
}




void Send_Tell_UserID(void *args)
{
	NetEvtClient *pNetEventClient = (NetEvtClient *)args;
	char* userid = "user0001";

	MessagePackage msgPackage;
	msgPackage.WriteHeader(ID_User_Login, c2s_tell_user_id);
	msgPackage.WriteBody(&userid, strlen(userid));
	pNetEventClient->Send(msgPackage);

	LOG(info, "向服务器发送userID:[%s]\n", userid);
}



void Send_UserInfo(void *args)
{

	time_t t;
	srand((unsigned)time(&t));

	NetEvtClient *pNetEventClient = (NetEvtClient *)args;

	UserInfo userInfo;
	userInfo.SeatNumber  = random(10) + 1;
	memcpy(userInfo.UserID,  "wreewrewrew", sizeof("wreewrewrew"));
	MessagePackage msgPackage;
	msgPackage.WriteHeader(ID_User_Login, c2s_tell_user_info);
	msgPackage.WriteBody(&userInfo, sizeof(UserInfo));
	pNetEventClient->Send(msgPackage);

	LOG(info, "向服务器发送用户信息");
}




void Send_Seen_External(void *args)
{

	time_t t;
	srand((unsigned)time(&t));

	NetEvtClient *pNetEventClient = (NetEvtClient *)args;

	char* UserID =  "0001";
	int len = strlen(UserID);
	MessagePackage msgPackage;
	msgPackage.WriteHeader(ID_Global_Notify, c2s_seen_external);
	msgPackage.WriteBody((void*)UserID, len);
	pNetEventClient->Send(msgPackage);

	LOG(info, "向外部场景服务器发送UserID号:[%s]\n", UserID);
}

