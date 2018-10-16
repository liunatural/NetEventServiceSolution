#include "PlayerManager.h"
#include <assert.h>

PlayerManager::PlayerManager()
{
	//bigDataPkt = CreateBigPackage();

	SetUserVisibilityExternal(false);
}


PlayerManager::~PlayerManager()
{
}


void PlayerManager::AddPlayer(Player* ply)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(ply);
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
		if (NULL != ply && ply->GetUserType() == VIP)
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



bool PlayerManager::SendPlayerLeaveMsg(int& plyId)
{
	LinkID linkID;
	Player* ply = NULL;
	ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[SendPlayerLeaveMsg] error�����ID���б��в����ڣ�");
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
		LOG(error, "[UpdatePlayerTransform] �洢���λ�ñ任���ݳ������ID���б��в����ڣ�");
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
		LOG(error, "[UpdatePlayerSeatNumber] ���������λ�ų������ID���б��в����ڣ�");
		return false;
	}

	ply->SetSeatNumber(seatNumber);

	return true;
}


bool PlayerManager::UpdateUserType(int plyId, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	Player* ply = FindPlayer(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdatePlayerSeatNumber] ����������ͳ������ID���б��в����ڣ�");
		return false;
	}

	ply->SetUserType(userType);

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
	memcpy(usrStatInfo.SceneServerID, mSceneServerID, SCENE_SERVER_ID_LENGTH);
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


//int PlayerManager::SendFlvStream(char* data, int len)
//{
//	LinkID linkID;
//	Player* user = NULL;
//	bigDataPkt->SetBigPackage(ID_FLV_Stream, 0, data, len);
//
//	boost::mutex::scoped_lock lock(mMutex);
//	for (iterator i = begin(); i != end(); i++)
//	{
//
//		user = (Player*)(*i);
//		if (NULL != user)
//		{
//			//�����û�з��͹�SequenceHeader���ݣ�����SequenceHeader���ݱ��ˣ��ͷ���֮ 
//			if (!user->GetFlvSeqHeaderFlag() || true == flvSeqHeaderChange)
//			{
//				SendFlvSeqHeaderData(user);
//				flvSeqHeaderChange = false;
//			}
//
//			linkID = user->GetLinkID();
//			mpService->Send(linkID, *bigDataPkt);
//		}
//	}
//
//	return 1;
//}

//bool PlayerManager::SendFlvSeqHeaderData(Player* user)
//{
//	bool ret = true;
//
//	LinkID linkID = user->GetLinkID();
//	HXBigMessagePackage* bigSeqHeaderPkt = CreateBigPackage();
//
//	map<uint8_t, FlvSeqHeader>::iterator it;
//	it = flvSeqHeaderMap.begin();
//
//	while (it != flvSeqHeaderMap.end())
//	{
//		bigSeqHeaderPkt->SetBigPackage(ID_FLV_Sequence_Header, 0, it->second.data, it->second.size);
//		ret = mpService->Send(linkID, *bigSeqHeaderPkt);
//		if (!ret)
//		{
//			break;
//		}
//
//		it++;
//	}
//
//	if (ret)
//	{
//		user->SetFlvSeqHeaderFlag(true);
//	}
//
//	delete bigSeqHeaderPkt;
//	return ret;
//}


//void PlayerManager::CacheFlvSeqHeaderData(unsigned char* data, int len)
//{
//
//	if (!data || len <= 0)
//	{
//		return;
//	}
//
//	uint8_t dt = data[0];
//
//	boost::mutex::scoped_lock lock(mMutex);
//	FlvSeqHeaderMap::iterator it = flvSeqHeaderMap.find(dt);
//	if (it != flvSeqHeaderMap.end())
//	{
//		delete (*it).second.data;
//		flvSeqHeaderMap.erase(it);
//
//		flvSeqHeaderChange = true;
//	}
//
//	unsigned char* buf = new unsigned char[len];
//	FlvSeqHeader confData;
//	confData.id = data[0];
//	confData.data = buf;
//	confData.size = len;
//	memcpy(buf, data, len);
//
//	flvSeqHeaderMap.insert(make_pair(dt, confData));
//
//}


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
