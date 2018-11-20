
#include "NetEventService.h"
#include "VRSceneController.h"
#include <thread>

extern VRSceneController* g_pSceneCtrl;

int main()
{
	int ret = 0;

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/SceneController");


	VRSceneController vrSceneController;

	g_pSceneCtrl = &vrSceneController;

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
	ret= vrSceneController.CreatePlayerManager();
	if (ret == FAIL)
	{
		return ret;
	}



	//*****������Ϣ����*****//
	vrSceneController.Run();

    return SUCCESS;
}

