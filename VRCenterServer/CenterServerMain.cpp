#include "NetEventService.h"
#include "VRCenterSvr.h" 

int main()
{
	int ret = 0;

	//*****开启日志系统文件*****//
	InitLogger("Log/CenterServer");


	VRCenterSvr vrCentServer;

	//*****读取配置文件*******//
	ret = vrCentServer.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****启动中心服务器*****//
	ret = vrCentServer.Start();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****创建用户管理器*****//
	ret = vrCentServer.CreateUserManager();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****进行消息处理*****//
	vrCentServer.HandleNetEvent();

	return SUCCESS;
}

