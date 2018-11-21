
#include "NetEventService.h"
#include "VRSceneController.h"
#include <thread>

int main()
{
	int ret = 0;

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/SceneController");


	VRSceneController vrSceneController;


	//*****��ȡ�����ļ�*******//
	ret  = vrSceneController.ReadConfigFile();
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

