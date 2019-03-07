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

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/VRHostController");


	VRHostController vrHostController;


	//*****��ȡ�����ļ�*******//
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

	//*****����VR����������*****//
	ret = vrHostController.Start();
	if (ret == FAIL)
	{
		return ret;
	}




	//*****�����û�������*****//
	ret= vrHostController.CreateVRClientManager();
	if (ret == FAIL)
	{
		return ret;
	}



	//*****������Ϣ����*****//
	vrHostController.Run();

    return SUCCESS;
}

