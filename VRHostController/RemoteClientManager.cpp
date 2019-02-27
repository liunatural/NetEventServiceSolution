//**************************************************************************
//  File......... : RemoteClientManager.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Implementation file of the class RemoteClientManager used as the remote client  
//							management of VR host controller.
//  History...... : First created by Liu Zhi 2018-11
//
//***************************************************************************


#include "RemoteClientManager.h"
#include "VRHostController.h"


RemoteClientManager::RemoteClientManager()
{

}


RemoteClientManager::~RemoteClientManager()
{
}


void RemoteClientManager::AddRemoteClient(RemoteClient* client)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(client);

	LOG(info, "�ն�ID[%d]���룡", client->GetLinkID());

}


bool RemoteClientManager::DeleteRemoteClient(int& clientID)
{
	bool ret = false;
	RemoteClient* client = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		client = (RemoteClient*)(*i);
		if (NULL != client && (client->GetLinkID() == clientID))
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




//bool VRClientManager::DeleteRemoteClient(int& clientID)
//{
//	VRClient* client = NULL;
//	client = FindClient(clientID);
//	if (NULL == client)
//	{
//		LOG(error, "[DeleteRemoteClient] �����ն�ID[%d]�����ڣ�", clientID);
//		return false;
//	}
//
//
//	DeleteVRClient(clientID);
//
//	return true;
//
//}

bool RemoteClientManager::AssignSeatNumberToClient(int clientID, int seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	RemoteClient* client = FindClient(clientID);
	if (NULL == client)
	{
		LOG(error, "[AssignSeatNumberToClient] ָ����λ�Ÿ�Զ���ն˶�������ն�ID[%d]�����ڣ�", clientID);
		return false;
	}

	client->SetSeatNumber(seatNumber);

	RecreateUserSeatMap(client);

	return true;
}


void RemoteClientManager::RecreateUserSeatMap(RemoteClient* pClient)
{
	int seatNumber = pClient->GetSeatNumber();

	if (seatNumber == -1)
	{
		LOG(error, "���û����֣��ն�ID[%d]��ǰ����λ����Ч��", pClient->GetLinkID());
		return;
	}

	User_Seat_Map userSeatMap = mpSceneCtrl->GetUserSeatMap();
	User_Seat_Map::iterator it;

	it = userSeatMap.find(seatNumber);

	if (it != userSeatMap.end())
	{
		string userID = it->second;
		pClient->AssignUserID((char*)userID.c_str(), userID.size());

		pClient->SetUserBindFlag(true);

	}
}


bool RemoteClientManager::ReclaimRemoteClient(int clientID, int seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	RemoteClient* client = FindClient(clientID);
	if (NULL == client || client->GetSeatNumber() != seatNumber)
	{
		LOG(error, "[ReclaimRemoteClient] ����Զ���ն˳����ն�ID[%d]����ϯ��[%d] �����ڣ�", clientID, seatNumber);
		return false;
	}

	client->AssignUserID(NULL, 0);
	client->SetUserBindFlag(false);

	return true;

}


bool RemoteClientManager::BindUserIDToRemoteClient( char* userid, int len, RemoteClient** ppClient)
{
	boost::mutex::scoped_lock lock(mMutex);

	*ppClient = GetFreeRemoteClient();
	if (NULL == *ppClient)
	{
		LOG(error, "[BindUserIDToRemoteClient] ���û�ID����û�з��ֿ��õ�Զ���ն˶���");
		return false;
	}

	(*ppClient)->AssignUserID(userid, len);
	(*ppClient)->SetUserBindFlag(true);

	return true;
}



bool RemoteClientManager::UpdateClientType(int clientID, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	RemoteClient* ply = FindClient(clientID);
	if (NULL == ply)
	{
		LOG(error, "[UpdateClientType] ���¿ͻ������ͳ����ն�ID[%d]�����ڣ�", clientID);
		return false;
	}

	ply->SetClientType(userType);

	return true;
}



bool RemoteClientManager::SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len)
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

	return SendMsg(linkID, msgPackage);
}


bool RemoteClientManager::SendMsg(LinkID& linkID, const MessagePackage& msgPackage)
{
		return mpService->Send(linkID, (MessagePackage)msgPackage);
}






////////////////////////////////////////////////////////////////////////////////////////////////////////
RemoteClient* RemoteClientManager::FindClient(int clientID)
{
	RemoteClient* client = NULL;
	RemoteClient* clientTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		clientTemp = (RemoteClient*)(*i);
		if (clientTemp->GetLinkID() == clientID)
		{
			client = clientTemp;
			break;
		}
	}

	return client;
}


RemoteClient*  RemoteClientManager::GetFreeRemoteClient()
{
	RemoteClient* client				= NULL;
	RemoteClient* clientTemp		= NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		clientTemp = (RemoteClient*)(*i);
		if (NULL != clientTemp && !(clientTemp->IsBoundUser()) && (clientTemp->GetClientType() == VRClientAgent) )
		{
			client = clientTemp;
			break;
		}
	}

	return client;
}
