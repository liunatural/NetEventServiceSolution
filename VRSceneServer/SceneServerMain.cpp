
#include "NetEventService.h"
#include "VRSceneServer.h"
#include "CommonDef.h"
#include  "SvcTimer.h"
#include <thread>

void Timer_SendTransformData(VRSceneServer *pSceneServer)
{
	io_service io;
	SvcTimer p(io, pSceneServer->GetPlayerManager());
	io.run();
}



int main()
{
	int ret = 0;

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/SceneServer");


	VRSceneServer vrSceneServer;

	//*****��ȡ�����ļ�*******//
	ret  = vrSceneServer.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****��������������*****//
	ret = vrSceneServer.Start();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****�����û�������*****//
	ret= vrSceneServer.CreatePlayerManager();
	if (ret == FAIL)
	{
		return ret;
	}


	//*****������ʱ���������λ�ñ任��Ϣ*****//
	std::thread tr(&Timer_SendTransformData, &vrSceneServer);
	tr.detach();


	//*****������Ϣ����*****//
	vrSceneServer.HandleNetEvent();

    return SUCCESS;
}

