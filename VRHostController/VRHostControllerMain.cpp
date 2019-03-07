//**************************************************************************
//  File......... : VRHostControllerMain.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : main file of VR Host Controller server.
//  History...... :	 First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
//***************************************************************************

#include "NetEventService.h"
#include "VRHostController.h"
#include <thread>

int main()
{
	int ret = 0;

	//*****开启日志系统文件*****//
	InitLogger("Log/VRHostController");


	VRHostController vrHostController;


	//*****读取配置文件*******//
	ret  = vrHostController.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}


	ret = vrHostController.CreateUserSeatMap();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****启动VR主机控制器*****//
	ret = vrHostController.Start();
	if (ret == FAIL)
	{
		return ret;
	}




	//*****创建用户管理器*****//
	ret= vrHostController.CreateVRClientManager();
	if (ret == FAIL)
	{
		return ret;
	}



	//*****进行消息处理*****//
	vrHostController.Run();

    return SUCCESS;
}

