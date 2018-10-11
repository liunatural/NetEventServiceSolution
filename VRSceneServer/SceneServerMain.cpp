
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

	//*****开启日志系统文件*****//
	InitLogger("Log/SceneServer");


	VRSceneServer vrSceneServer;

	//*****读取配置文件*******//
	ret  = vrSceneServer.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****启动场景服务器*****//
	ret = vrSceneServer.Start();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****创建用户管理器*****//
	ret= vrSceneServer.CreatePlayerManager();
	if (ret == FAIL)
	{
		return ret;
	}


	//*****启动定时器处理玩家位置变换信息*****//
	std::thread tr(&Timer_SendTransformData, &vrSceneServer);
	tr.detach();


	//*****进行消息处理*****//
	vrSceneServer.HandleNetEvent();

    return SUCCESS;
}

