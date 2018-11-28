#include "VRClientManager.h"
#include "VRSceneController.h"


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

	LOG(info, "终端ID[%d]加入！", client->GetLinkID());

}



bool VRClientManager::DeleteVRClientFromList(int& clientID)
{
	VRClient* client = NULL;
	client = FindVRClient(clientID);
	if (NULL == client)
	{
		LOG(error, "[DeleteVRClientFromList] error：终端ID[%d]不存在！", clientID);
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
		LOG(error, "[UpdateSeatNumber] 更新座位号出错：终端ID[%d]不存在！", clientID);
		return false;
	}

	client->SetSeatNumber(seatNumber);

	CheckUserSeatMap(client);

	return true;
}


void VRClientManager::CheckUserSeatMap(VRClient* pClient)
{
	int seatNumber = pClient->GetSeatNumber();

	User_Seat_Map userSeatMap = mpSceneCtrl->GetUserSeatMap();
	User_Seat_Map::iterator it;

	it = userSeatMap.find(seatNumber);

	if (it != userSeatMap.end())
	{
		string userID = it->second;
		pClient->SetUserID((char*)userID.c_str(), userID.size());

		pClient->SetBoundUser(true);

	}
}

bool VRClientManager::BindUserIDToVRClient( char* userid, int len, VRClient** client)
{
	boost::mutex::scoped_lock lock(mMutex);

	*client = GetFreeVRClient();
	if (NULL == *client)
	{
		LOG(error, "[BindUserIDToVRClient] 绑定userID出错：没有空闲的座席号供分配了！");
		return false;
	}

	(*client)->SetUserID(userid, len);


	(*client)->SetBoundUser(true);

	return true;
}



bool VRClientManager::UpdateClientType(int clientID, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	VRClient* ply = FindVRClient(clientID);
	if (NULL == ply)
	{
		LOG(error, "[UpdateClientType] 更新客户端类型出错：终端ID[%d]不存在！", clientID);
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

	return SendMsg(linkID, msgPackage);
}


bool VRClientManager::SendMsg(LinkID& linkID, const MessagePackage& msgPackage)
{
		return mpService->Send(linkID, (MessagePackage)msgPackage);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

VRClient* VRClientManager::FindVRClient(int clientID)
{
	VRClient* client = NULL;
	VRClient* clientTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		clientTemp = (VRClient*)(*i);
		if (clientTemp->GetLinkID() == clientID)
		{
			client = clientTemp;
			break;
		}
	}

	return client;
}


VRClient*  VRClientManager::GetFreeVRClient()
{
	VRClient* client				= NULL;
	VRClient* clientTemp		= NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		clientTemp = (VRClient*)(*i);
		if (NULL != clientTemp && !(clientTemp->BoundUser()) && (clientTemp->GetUserType() == VRClientAgent) )
		{
			client = clientTemp;
			break;
		}
	}

	return client;
}


bool VRClientManager::DeleteVRClient(int clientID)
{
	bool ret = false;
	VRClient* client = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		client = (VRClient*)(*i);
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
