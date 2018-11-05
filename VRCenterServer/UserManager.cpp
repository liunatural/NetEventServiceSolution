#include "UserManager.h"

UserManager::UserManager()
{
}


UserManager::~UserManager()
{
}


void UserManager::AddUser(User* ply)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(ply);
}


void UserManager::SetNetworkService(NetEvtServer** pService)
{
	mpService = *pService;
}


int UserManager::ForwardCommand(MessagePackage *pkt)
{
	LinkID linkID;
	User* user = NULL;

	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		user = (User*)(*i);
		if (NULL != user)
		{
			linkID = user->GetLinkID();
			mpService->Send(linkID, *pkt);
		}
	}

	return 1;
}


int UserManager::ForwardMsgToOtherSceneServers(LinkID& sourceLinkID, MessagePackage *pkt)
{
	LinkID linkID;
	User* user = NULL;

	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		user = (User*)(*i);
		if (NULL != user)
		{
			linkID = user->GetLinkID();
			if (sourceLinkID != linkID)//向除消息源用户外的其它用户转发此消息
			{
				mpService->Send(linkID, *pkt);
			}
		}
	}

	return 1;
}


int  UserManager::SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len)
{
	return mpService->Send(linkID, msgID, cmdID, data, len);
}


User* UserManager::FindUser(int plyId)
{
	User* ply = NULL;
	User* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (User*)(*i);
		if (plyTemp->mUserID == plyId)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


bool UserManager::DeleteUser(int plyId)
{
	bool ret = false;
	User* ply = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		ply = (User*)(*i);
		if (NULL != ply && (ply->mUserID == plyId))
		{
			delete ply;
			ply = NULL;
			this->erase(i);

			ret = true;
			break;
		}
	}

	return ret;
}
