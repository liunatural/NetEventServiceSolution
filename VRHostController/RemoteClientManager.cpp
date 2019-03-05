//**************************************************************************
//  File......... : RemoteClientManager.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-11 
//  Description.. : Implementation file of the class RemoteClientManager used as the remote client  
//							management of VR host controller.
//  History...... : First created by Liu Zhi 2018-11
//								update by Liu Zhi 2019-02
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
	boost::mutex::scoped_lock lock(m_RCMgrMutex);
	push_back(client);

	memset(msgBuf, 0, sizeof(msgBuf));
	sprintf(msgBuf, "[%s]�ɹ�����VR����������������", client->GetIP());
	m_outLog.m_logType = LOG_TYPE::info;
	m_outLog.m_logStr = msgBuf;
}

bool RemoteClientManager::DeleteRemoteClient(int& clientID)
{
	bool ret = false;
	RemoteClient* client = NULL;
	boost::mutex::scoped_lock lock(m_RCMgrMutex);
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

bool RemoteClientManager::AssignSeatNumToVRHost(int clientID, int seatNumber)
{
	boost::mutex::scoped_lock lock(m_RCMgrMutex);

	RemoteClient* client = FindClient(clientID);
	if (NULL == client)
	{
		LOG(error, "[AssignSeatNumberToClient] ����VR����[%d]�����ڣ�", clientID);

		return false;
	}

	client->SetSeatNumber(seatNumber);

	RecreateUserSeatMap(client);

	return true;
}

void RemoteClientManager::RecreateUserSeatMap(RemoteClient* pVRHost)
{
	int seatNumber = pVRHost->GetSeatNumber();

	if (seatNumber == -1)
	{
		LOG(error, "[RecreateUserSeatMap]����: VR������ǰ����λ��[%d]��Ч��", seatNumber);
		return;
	}

	User_Seat_Map userSeatMap = m_pHostCtlr->GetUserSeatMap();
	User_Seat_Map::iterator it;

	it = userSeatMap.find(seatNumber);

	if (it != userSeatMap.end())
	{
		string userID = it->second;
		pVRHost->AssignUserID((char*)userID.c_str(), userID.size());

		pVRHost->SetUserBindFlag(true);
	}
}

bool RemoteClientManager::ReclaimVRHost(int clientID, int seatNumber)
{
	boost::mutex::scoped_lock lock(m_RCMgrMutex);

	RemoteClient* pVRHostObj = FindClient(clientID);
	if (NULL == pVRHostObj || pVRHostObj->GetSeatNumber() != seatNumber)
	{
		LOG(error, "[ReclaimRemoteClient] ����VR����ID[%d]����ϯ��[%d] �����ڣ�", clientID, seatNumber);
		return false;
	}

	pVRHostObj->AssignUserID(NULL, 0);
	pVRHostObj->SetUserBindFlag(false);

	return true;

}

bool RemoteClientManager::AllocateVRHostForUser( char* userid, int len, RemoteClient** ppVRHostObj)
{
	boost::mutex::scoped_lock lock(m_RCMgrMutex);

	*ppVRHostObj = GetFreeRemoteClient();
	if (NULL == *ppVRHostObj)
	{
		LOG(error, "[AllocateVRHostForUser]���� û�п��е�VR�����ṩ���û�[%s]ʹ�ã�", userid);
		return false;
	}

	(*ppVRHostObj)->AssignUserID(userid, len);
	(*ppVRHostObj)->SetUserBindFlag(true);

	return true;
}

bool RemoteClientManager::UpdateClientType(int clientID, UserType userType)
{

	boost::mutex::scoped_lock lock(m_RCMgrMutex);
	RemoteClient* ply = FindClient(clientID);
	if (NULL == ply)
	{
		LOG(error, "[UpdateClientType]�����ͻ���[%d]�����ڣ�", clientID);
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
		return m_pNetEvSvc->Send(linkID, (MessagePackage)msgPackage);
}

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
