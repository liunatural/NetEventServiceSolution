//**************************************************************************
//  File......... : VRSceneControllerMain.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : main file of VR Scene Controller server.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************

#include "NetEventService.h"
#include "VRHostController.h"
#include <thread>

int main()
{
	int ret = 0;

	//*****开启日志系统文件*****//
	InitLogger("Log/SceneController");


	VRHostController vrSceneController;


	//*****读取配置文件*******//
	ret  = vrSceneController.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}


	ret = vrSceneController.CreateUserSeatMap();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****启动场景服务器*****//
	ret = vrSceneController.Start();
	if (ret == FAIL)
	{
		return ret;
	}




	//*****创建用户管理器*****//
	ret= vrSceneController.CreateVRClientManager();
	if (ret == FAIL)
	{
		return ret;
	}



	//*****进行消息处理*****//
	vrSceneController.Run();

    return SUCCESS;
}

