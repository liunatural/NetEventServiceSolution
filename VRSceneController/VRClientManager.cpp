#include "VRClientManager.h"

VRClientManager::VRClientManager()
{

}


VRClientManager::~VRClientManager()
{
}


void VRClientManager::AddVRClient(VRClient* client)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(client);

	LOG(info, "�ն�ID[%d]���룡", client->GetLinkID());

}



bool VRClientManager::DeleteVRClientFromList(int& clientID)
{
	VRClient* client = NULL;
	client = FindVRClient(clientID);
	if (NULL == client)
	{
		LOG(error, "[DeleteVRClientFromList] error���ն�ID[%d]�����ڣ�", clientID);
		return false;
	}


	DeleteVRClient(clientID);

	return true;

}

bool VRClientManager::UpdateSeatNumber(int clientID, int seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	VRClient* client = FindVRClient(clientID);
	if (NULL == client)
	{
		LOG(error, "[UpdateSeatNumber] ������λ�ų����ն�ID[%d]�����ڣ�", clientID);
		return false;
	}

	client->SetSeatNumber(seatNumber);

	return true;
}


bool VRClientManager::BindUserIDToVRClient( char* userid, int len, int &seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	VRClient* client = GetFreeVRClient();
	if (NULL == client)
	{
		LOG(error, "[BindUserIDToVRClient] ��userID����û�п��е���ϯ�Ź������ˣ�");
		return false;
	}

	client->SetUserID(userid, len);
	seatNumber = client->GetSeatNumber();

	return true;
}



bool VRClientManager::UpdateClientType(int clientID, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	VRClient* ply = FindVRClient(clientID);
	if (NULL == ply)
	{
		LOG(error, "[UpdateClientType] ���¿ͻ������ͳ����ն�ID[%d]�����ڣ�", clientID);
		return false;
	}

	ply->SetUserType(userType);

	return true;
}



bool VRClientManager::SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len)
{
	MessagePackage msgPackage;
	msgPackage.header()->id1 = msgID;
	msgPackage.header()->id2 = cmdID;
	msgPackage.header()->length = 0;

	if ((NULL != data) && (0 != len))
	{
		msgPackage.header()->length = len;
		memcpy(msgPackage.body(), data, len);
	}

	return mpService->Send(linkID, msgPackage);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

VRClient* VRClientManager::FindVRClient(int clientID)
{
	VRClient* client = NULL;
	VRClient* clientTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		clientTemp = (VRClient*)(*i);
		if (clientTemp->mClientID == clientID)
		{
			client = clientTemp;
			break;
		}
	}

	return client;
}


VRClient*  VRClientManager::GetFreeVRClient()
{
	VRClient* ply				= NULL;
	VRClient* plyTemp		= NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (VRClient*)(*i);
		if (NULL != plyTemp && !(plyTemp->bBoundUser) && (plyTemp->GetUserType() == VRClientAgent) )
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


bool VRClientManager::DeleteVRClient(int clientID)
{
	bool ret = false;
	VRClient* client = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		client = (VRClient*)(*i);
		if (NULL != client && (client->mClientID == clientID))
		{
			delete client;
			client = NULL;
			this->erase(i);

			ret = true;
			break;
		}
	}

	return ret;
}
