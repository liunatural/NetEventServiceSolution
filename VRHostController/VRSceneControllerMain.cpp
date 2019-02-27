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

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/SceneController");


	VRHostController vrSceneController;


	//*****��ȡ�����ļ�*******//
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

	//*****��������������*****//
	ret = vrSceneController.Start();
	if (ret == FAIL)
	{
		return ret;
	}




	//*****�����û�������*****//
	ret= vrSceneController.CreateVRClientManager();
	if (ret == FAIL)
	{
		return ret;
	}



	//*****������Ϣ����*****//
	vrSceneController.Run();

    return SUCCESS;
}

