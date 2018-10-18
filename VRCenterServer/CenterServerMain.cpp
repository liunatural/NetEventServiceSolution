#include "NetEventService.h"
#include "VRCenterSvr.h" 

int main()
{
	int ret = 0;

	//*****������־ϵͳ�ļ�*****//
	InitLogger("Log/CenterServer");


	VRCenterSvr vrCentServer;

	//*****��ȡ�����ļ�*******//
	ret = vrCentServer.ReadConfigFile();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****�������ķ�����*****//
	ret = vrCentServer.Start();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****�����û�������*****//
	ret = vrCentServer.CreateUserManager();
	if (ret == FAIL)
	{
		return ret;
	}

	//*****������Ϣ����*****//
	vrCentServer.HandleNetEvent();

	return SUCCESS;
}

