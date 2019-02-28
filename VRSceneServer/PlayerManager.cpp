#include "PlayerManager.h"
#include <assert.h>
#include <thread>

PlayerManager::PlayerManager()
{
	SetUserVisibilityExternal(false);

	mpConnToCenterSvr = NULL;

}


PlayerManager::~PlayerManager()
{
}


void PlayerManager::AddPlayer(Player* ply)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(ply);

	LOG(info, "���ID[%d]���룡", ply->GetLinkID());

	//ListPlayer();

}

void PlayerManager::SendClientList(LinkID& linkID)
{
	//Ϊ�˷�ֹ�ڷ������ݰ�ʱ������˳������������� ������ط�����
	boost::mutex::scoped_lock lock(mMutex);

	memset(user_list_buffer, 0, sizeof(user_list_buffer));
	char*p = user_list_buffer;
	int count = 0;

	Player* ply = NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		if ((sizeof(user_list_buffer) - count * sizeof(ProfileInfo)) < sizeof(ProfileInfo))
		{
			break;
		}

		ply = (Player*)(*i);
		if (NULL != ply && ply->GetUserType() == VIP && (ply->GetLinkID() != linkID))
		{
			memcpy(p, (const void*)&ply->mProfileInfo, sizeof(ProfileInfo));

			p += sizeof(ProfileInfo);
			count++;
		}
	}

	if (count > 0)
	{
		SendCmd(linkID, ID_User_Notify, s2c_client_list, user_list_buffer, sizeof(ProfileInfo)* count);
	}


}

void PlayerManager::SendClientListToCenterServer()
{
	//Ϊ�˷�ֹ�ڷ������ݰ�ʱ������˳������������� ������ط�����
	boost::mutex::scoped_lock lock(mMutex);

	memset(user_list_buffer, 0, sizeof(user_list_buffer));
	char*p = user_list_buffer;
	int count = 0;

	Player* ply = NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		if ((sizeof(user_list_buffer) - count * sizeof(ProfileInfo)) < sizeof(ProfileInfo))
		{
			break;
		}

		ply = (Player*)(*i);
		if (NULL != ply && ply->GetUserType() == VIP && ply->GetSeatNumber() > 0)
		{
			memcpy(p, (const void*)&ply->mProfileInfo, sizeof(ProfileInfo));

			p += sizeof(ProfileInfo);
			count++;
		}
	}

	if (count > 0)
	{
		mpConnToCenterSvr->Send(ID_Global_Notify, s2c_client_list_external, (const char*)user_list_buffer, sizeof(ProfileInfo)* count);
	}

}



void PlayerManager::SendUserInfoList(LinkID& linkID)
{
	//Ϊ�˷�ֹ�ڷ������ݰ�ʱ������˳������������� ������ط�����
	boost::mutex::scoped_lock lock(mMutex);

	memset(user_list_buffer, 0, sizeof(user_list_buffer));
	char*p = user_list_buffer;
	int count = 0;

	Player* ply = NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		if ((sizeof(user_list_buffer) - count * sizeof(UserInfo)) < sizeof(UserInfo))
		{
			break;
		}

		ply = (Player*)(*i);
		if (NULL != ply && ply->GetUserType() == VIP && (ply->GetLinkID() != linkID))
		{
			
			UserInfo usrInfo;
			usrInfo.SeatNumber = ply->GetSeatNumber();
			memcpy(usrInfo.UserID, ply->mUserID, strlen(ply->mUserID));
			memcpy(p, (const void*)&usrInfo, sizeof(UserInfo));

			p += sizeof(UserInfo);
			count++;
		}
	}

	if (count > 0)
	{
		SendCmd(linkID, ID_User_Notify, s2c_client_list, user_list_buffer, sizeof(UserInfo)* count);
	}


}



bool PlayerManager::SendPlayerLeaveMsg(int& plyId)
{
	LinkID linkID;
	Player* ply = NULL;
	ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[SendPlayerLeaveMsg] error�����ID[%d]���б��в����ڣ�", plyId);
		return false;
	}

	int seatNum = ply->GetSeatNumber();

	for (iterator i = begin(); i != end(); i++)
	{
		ply = (Player*)(*i);
		if (NULL != ply)
		{
			linkID = ply->GetLinkID();
			SendCmd(linkID, ID_User_Notify, s2c_ply_leave, &seatNum, sizeof(int));
		}
	}

	//if (GetUserVisibilityExternal())
	//{
	//	LeaveUserInfo leaveUserInfo;
	//	memcpy(leaveUserInfo.SceneServerID, ply->GetSceneServerID(), SCENE_SERVER_ID_LENGTH);
	//	leaveUserInfo.mSeatNumber = seatNum;

	//	mpConnToCenterSvr->Send(ID_Global_Notify, s2c_user_leave_external, (const char*)&leaveUserInfo, sizeof(LeaveUserInfo));
	//}


	DeletePlayer(plyId);

	return true;

}

void PlayerManager::UpdatePlayerTransform(int plyId, TransformInfo &transInfo)
{

	boost::mutex::scoped_lock lock(mMutex);
	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdatePlayerTransform] �洢���λ�ñ任���ݳ������ID[%d]���б��в����ڣ�", plyId);
		//ListPlayer();
		return;
	}

	ply->transInfo = transInfo;
	ply->transInfo.update = true;

}

bool PlayerManager::UpdatePlayerSeatNumber(int plyId, int seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdatePlayerSeatNumber] ���������λ�ų������ID[%d]���б��в����ڣ�", plyId);
		return false;
	}

	ply->SetSeatNumber(seatNumber);

	return true;
}


bool PlayerManager::BindUserIDToPlayer(int plyId, char* userid, int len)
{
	boost::mutex::scoped_lock lock(mMutex);

	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[BindUserIDToPlayer] ��userID�������ID[%d]���б��в����ڣ�", plyId);
		return false;
	}

	ply->SetUserID(userid, len);

	return true;
}



bool PlayerManager::UpdateUserInfo(int plyId, UserInfo *usrInfo)
{
	boost::mutex::scoped_lock lock(mMutex);

	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdatePlayerSeatNumber] ���������Ϣ�������ID[%d]���б��в����ڣ�", plyId);
		return false;
	}

	ply->SetSeatNumber(usrInfo->SeatNumber);
	ply->SetUserID(usrInfo->UserID, strlen(usrInfo->UserID));
	return true;
}




bool PlayerManager::UpdateUserType(int plyId, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdateUserType] ����������ͳ������ID[%d]���б��в����ڣ�", plyId);
		return false;
	}

	ply->SetUserType(userType);

	return true;
}


bool PlayerManager::UpdateUserTypeByUserID(char* userid, int len, UserType userType)
{
	if (len <= 0 || userid==NULL)
	{
		LOG(error, "[UpdateUserTypeByUserID] ����������Ͳ�������");
		return false;
	}

	if (len > USER_ID_LENGTH)
	{
		len = USER_ID_LENGTH;
	}

	char uidBuf[USER_ID_LENGTH + 1] = { 0 };
	memcpy(uidBuf, userid, len);

	{
		boost::mutex::scoped_lock lock(mMutex);
		Player* ply = FindPlayerByUserID(uidBuf);
		if (NULL == ply)
		{
			LOG(error, "[UpdateUserTypeByUserID] ����������ͳ���UserID[%s]���б��в����ڣ�", uidBuf);
			return false;
		}

		ply->SetUserType(userType);

	}
	return true;

}

void PlayerManager::BindFaceModeWithSeatNumber(LinkID& linkID, FaceModel* faceModel, int& plyId)
{
	boost::mutex::scoped_lock lock(mMutex);
	Player* ply = GetFreePlayer();
	if (NULL == ply)
	{
		SendCmd(linkID, ID_ERROR, NO_AVAILABLE_SEAT_NUMBER, NULL, 0);
		LOG(error, "[SendSeatNumberMsg]������ϯ�ų���û�п��õ���ϯ�����䣡");
		return;
	}

	int seatNum = ply->GetSeatNumber();
	//������ͻ��˷��ظ�����ĵ�ǰ���κ�
	SendCmd(linkID, ID_User_Login, s2c_rsp_seat_num, &seatNum, sizeof(int));
	
	ply->mProfileInfo.mFaceModel = *faceModel;
	
	//����Ϊ���û�״̬
	ply->bBoundUser = true;

	//�����û�ID
	plyId = ply->mPlyID;

}

void PlayerManager::SendUserReadyMsg(int seatNumber)
{
	LinkID linkID;
	Player* ply = NULL;
	Player* ply1 = NULL;

	ply = FindPlayerBySeatNumber(seatNumber);
	if (NULL == ply)
	{
		LOG(error, "[SendProfileUpdateMsg] �㲥��һ�����Ϣ�������ID���б��в����ڣ�");
		return;
	}

	if (ply->GetUserType() != VIP)//ֻ�㲥VIP�����û���״̬
	{
		return;
	}

	for (iterator i = begin(); i != end(); i++)
	{
		ply1 = (Player*)(*i);
		if (NULL != ply1)
		{
			linkID = ply1->GetLinkID();
			SendCmd(linkID, ID_User_Login, s2c_ply_ready, &ply->mProfileInfo, sizeof(ProfileInfo));
		}
	}
}


void PlayerManager::BroadcastUserState(int plyId, int msgID, UserState userState)
{
	LinkID linkID;
	Player* ply = NULL;
	Player* ply1 = NULL;

	ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[BroadcastUserState] error�����ID���б��в����ڣ�");
		return;
	}

	if (ply->GetUserType() != VIP) //ֻ�㲥VIP�����û���״̬
	{
		return;
	}

	UserStateInfo usrStatInfo;
	memcpy(usrStatInfo.SceneServerID, mSceneServerID, SERVER_ID_LENGTH);
	usrStatInfo.seatNumber = ply->GetSeatNumber();
	usrStatInfo.userState = userState;

	for (iterator i = begin(); i != end(); i++)
	{
		ply1 = (Player*)(*i);
		if (NULL != ply1 )
		{
			linkID = ply1->GetLinkID();
			if (linkID == plyId)
			{
				continue;
			}

			SendCmd(linkID, msgID, s2c_upd_user_state, &usrStatInfo, sizeof(UserStateInfo));
		}
	}


	//if (GetUserVisibilityExternal())
	//{
	//	mpConnToCenterSvr->Send(msgID, s2c_upd_user_state, (const char*)&usrStatInfo, sizeof(UserStateInfo));
	//}

}


void PlayerManager::BroadcastExternalUserState(const MessagePackage* pack)
{
	SendMsg(*pack);
}


void PlayerManager::BroadcastControlCmd(int msgID, int cmdID)
{
	LinkID linkID;
	Player* ply = NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		ply = (Player*)(*i);
		if (NULL != ply && (ply->GetUserType() != Capsule))
		{
			linkID = ply->GetLinkID();
			SendCmd(linkID, msgID, cmdID, NULL, 0);
		}
	}
}


bool PlayerManager::SendMsg(const MessagePackage& msgPackage)
{
	LinkID linkID;
	Player* user = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		user = (Player*)(*i);
		if (NULL != user)
		{
			linkID = user->GetLinkID();
			mpService->Send(linkID, (MessagePackage)msgPackage);
		}
	}

	return true;
}


bool PlayerManager::SendCmd(LinkID& linkID, int msgID, int cmdID, void* data, int len)
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

Player* PlayerManager::FindPlayer(int plyId)
{
	Player* ply = NULL;
	Player* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (Player*)(*i);
		if (plyTemp->mPlyID == plyId)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


void PlayerManager::ListPlayer()
{

	Player* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (Player*)(*i);
		LOG(info, "ID: %d", plyTemp->GetLinkID());
	}
}

Player* PlayerManager::FindPlayerBySeatNumber(int seatNumber)
{
	Player* ply = NULL;
	Player* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (Player*)(*i);
		if (plyTemp->GetSeatNumber() == seatNumber)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


Player* PlayerManager::FindPlayerByUserID(char* userid)
{
	Player* ply = NULL;
	Player* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (Player*)(*i);
		if (strcmp(plyTemp->mUserID, userid) == 0)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}




Player*  PlayerManager::GetFreePlayer()
{
	Player* ply				= NULL;
	Player* plyTemp		= NULL;
	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (Player*)(*i);
		if (NULL != plyTemp && !(plyTemp->bBoundUser) && (plyTemp->GetUserType() == VIP) )
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


bool PlayerManager::DeletePlayer(int plyId)
{
	bool ret = false;
	Player* ply = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		ply = (Player*)(*i);
		if (NULL != ply && (ply->mPlyID == plyId))
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
