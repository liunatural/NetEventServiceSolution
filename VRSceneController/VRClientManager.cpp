#include "VRClientManager.h"
#include <assert.h>
#include <thread>

VRClientManager::VRClientManager()
{

}


VRClientManager::~VRClientManager()
{
}


void VRClientManager::AddVRClient(VRClient* ply)
{
	boost::mutex::scoped_lock lock(mMutex);
	push_back(ply);

	LOG(info, "终端ID[%d]加入！", ply->GetLinkID());

}

//void VRClientManager::SendClientList(LinkID& linkID)
//{
//	//为了防止在发送数据包时有玩家退出引起程序崩溃， 从这个地方加锁
//	boost::mutex::scoped_lock lock(mMutex);
//
//	memset(user_list_buffer, 0, sizeof(user_list_buffer));
//	char*p = user_list_buffer;
//	int count = 0;
//
//	VRClient* ply = NULL;
//	for (iterator i = begin(); i != end(); i++)
//	{
//		if ((sizeof(user_list_buffer) - count * sizeof(ProfileInfo)) < sizeof(ProfileInfo))
//		{
//			break;
//		}
//
//		ply = (VRClient*)(*i);
//		if (NULL != ply && ply->GetUserType() == VIP && (ply->GetLinkID() != linkID))
//		{
//			memcpy(p, (const void*)&ply->mProfileInfo, sizeof(ProfileInfo));
//
//			p += sizeof(ProfileInfo);
//			count++;
//		}
//	}
//
//	if (count > 0)
//	{
//		SendCmd(linkID, ID_User_Notify, s2c_client_list, user_list_buffer, sizeof(ProfileInfo)* count);
//	}
//}



bool VRClientManager::DeleteVRClientFromList(int& plyId)
{
	VRClient* ply = NULL;
	ply = FindVRClient(plyId);
	if (NULL == ply)
	{
		LOG(error, "[SendPlayerLeaveMsg] error：终端ID[%d]在列表中不存在！", plyId);
		return false;
	}


	DeleteVRClient(plyId);

	return true;

}

//void VRClientManager::UpdatePlayerTransform(int plyId, TransformInfo &transInfo)
//{
//
//	boost::mutex::scoped_lock lock(mMutex);
//	VRClient* ply = FindPlayer(plyId);
//	if (NULL == ply)
//	{
//		LOG(error, "[UpdatePlayerTransform] 存储玩家位置变换数据出错：玩家ID[%d]在列表中不存在！", plyId);
//		//ListPlayer();
//		return;
//	}
//
//	ply->transInfo = transInfo;
//	ply->transInfo.update = true;
//
//}

bool VRClientManager::UpdateSeatNumber(int plyId, int seatNumber)
{
	boost::mutex::scoped_lock lock(mMutex);

	VRClient* ply = FindVRClient(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdatePlayerSeatNumber] 更新座位号出错：终端ID[%d]在列表中不存在！", plyId);
		return false;
	}

	ply->SetSeatNumber(seatNumber);

	return true;
}


bool VRClientManager::BindUserIDToVRClient(int plyId, char* userid, int len)
{
	boost::mutex::scoped_lock lock(mMutex);

	VRClient* ply = FindVRClient(plyId);
	if (NULL == ply)
	{
		LOG(error, "[BindUserIDToPlayer] 绑定userID出错：玩家ID[%d]在列表中不存在！", plyId);
		return false;
	}

	ply->SetUserID(userid, len);

	return true;
}



bool VRClientManager::UpdateUserType(int plyId, UserType userType)
{

	boost::mutex::scoped_lock lock(mMutex);
	VRClient* ply = FindVRClient(plyId);
	if (NULL == ply)
	{
		LOG(error, "[UpdateUserType] 更新玩家类型出错：玩家ID[%d]在列表中不存在！", plyId);
		return false;
	}

	ply->SetUserType(userType);

	return true;
}


bool VRClientManager::UpdateUserTypeByUserID(char* userid, int len, UserType userType)
{
	if (len <= 0 || userid==NULL)
	{
		LOG(error, "[UpdateUserTypeByUserID] 更新玩家类型参数错误！");
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
		VRClient* ply = FindVRClientByUserID(uidBuf);
		if (NULL == ply)
		{
			LOG(error, "[UpdateUserTypeByUserID] 更新玩家类型出错：UserID[%s]在列表中不存在！", uidBuf);
			return false;
		}

		ply->SetUserType(userType);

	}
	return true;

}

//void VRClientManager::BindFaceModeWithSeatNumber(LinkID& linkID, FaceModel* faceModel, int& plyId)
//{
//	boost::mutex::scoped_lock lock(mMutex);
//	VRClient* ply = GetFreePlayer();
//	if (NULL == ply)
//	{
//		SendCmd(linkID, ID_ERROR, NO_AVAILABLE_SEAT_NUMBER, NULL, 0);
//		LOG(error, "[SendSeatNumberMsg]分配座席号出错：没有可用的座席供分配！");
//		return;
//	}
//
//	int seatNum = ply->GetSeatNumber();
//	//向胶囊体客户端返回刚申请的当前座椅号
//	SendCmd(linkID, ID_User_Login, s2c_rsp_seat_num, &seatNum, sizeof(int));
//	
//	ply->mProfileInfo.mFaceModel = *faceModel;
//	
//	//设置为绑定用户状态
//	ply->bBoundUser = true;
//
//	//返回用户ID
//	plyId = ply->mPlyID;
//
//}

//void VRClientManager::SendUserReadyMsg(int seatNumber)
//{
//	LinkID linkID;
//	VRClient* ply = NULL;
//	VRClient* ply1 = NULL;
//
//	ply = FindPlayerBySeatNumber(seatNumber);
//	if (NULL == ply)
//	{
//		LOG(error, "[SendProfileUpdateMsg] 广播玩家基本信息出错：玩家ID在列表中不存在！");
//		return;
//	}
//
//	if (ply->GetUserType() != VIP)//只广播VIP类型用户的状态
//	{
//		return;
//	}
//
//	for (iterator i = begin(); i != end(); i++)
//	{
//		ply1 = (VRClient*)(*i);
//		if (NULL != ply1)
//		{
//			linkID = ply1->GetLinkID();
//			SendCmd(linkID, ID_User_Login, s2c_ply_ready, &ply->mProfileInfo, sizeof(ProfileInfo));
//		}
//	}
//}


//void VRClientManager::BroadcastUserState(int plyId, int msgID, UserState userState)
//{
//	LinkID linkID;
//	VRClient* ply = NULL;
//	VRClient* ply1 = NULL;
//
//	ply = FindPlayer(plyId);
//	if (NULL == ply)
//	{
//		LOG(error, "[BroadcastUserState] error：玩家ID在列表中不存在！");
//		return;
//	}
//
//	if (ply->GetUserType() != VIP) //只广播VIP类型用户的状态
//	{
//		return;
//	}
//
//	UserStateInfo usrStatInfo;
//	memcpy(usrStatInfo.SceneServerID, mSceneServerID, SCENE_SERVER_ID_LENGTH);
//	usrStatInfo.seatNumber = ply->GetSeatNumber();
//	usrStatInfo.userState = userState;
//
//	for (iterator i = begin(); i != end(); i++)
//	{
//		ply1 = (VRClient*)(*i);
//		if (NULL != ply1 )
//		{
//			linkID = ply1->GetLinkID();
//			if (linkID == plyId)
//			{
//				continue;
//			}
//
//			SendCmd(linkID, msgID, s2c_upd_user_state, &usrStatInfo, sizeof(UserStateInfo));
//		}
//	}
//
//}


//void VRClientManager::BroadcastExternalUserState(const MessagePackage* pack)
//{
//	SendMsg(*pack);
//}


//void VRClientManager::BroadcastControlCmd(int msgID, int cmdID)
//{
//	LinkID linkID;
//	VRClient* ply = NULL;
//	for (iterator i = begin(); i != end(); i++)
//	{
//		ply = (VRClient*)(*i);
//		if (NULL != ply && (ply->GetUserType() != Capsule))
//		{
//			linkID = ply->GetLinkID();
//			SendCmd(linkID, msgID, cmdID, NULL, 0);
//		}
//	}
//}


bool VRClientManager::SendMsg(const MessagePackage& msgPackage)
{
	LinkID linkID;
	VRClient* user = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		user = (VRClient*)(*i);
		if (NULL != user)
		{
			linkID = user->GetLinkID();
			mpService->Send(linkID, (MessagePackage)msgPackage);
		}
	}

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

VRClient* VRClientManager::FindVRClient(int plyId)
{
	VRClient* ply = NULL;
	VRClient* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (VRClient*)(*i);
		if (plyTemp->mPlyID == plyId)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


//void VRClientManager::ListPlayer()
//{
//
//	VRClient* plyTemp = NULL;
//
//	for (iterator i = begin(); i != end(); i++)
//	{
//		plyTemp = (VRClient*)(*i);
//		LOG(info, "ID: %d", plyTemp->GetLinkID());
//	}
//}

VRClient* VRClientManager::FindVRClientBySeatNumber(int seatNumber)
{
	VRClient* ply = NULL;
	VRClient* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (VRClient*)(*i);
		if (plyTemp->GetSeatNumber() == seatNumber)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}


VRClient* VRClientManager::FindVRClientByUserID(char* userid)
{
	VRClient* ply = NULL;
	VRClient* plyTemp = NULL;

	for (iterator i = begin(); i != end(); i++)
	{
		plyTemp = (VRClient*)(*i);
		if (strcmp(plyTemp->mUserID, userid) == 0)
		{
			ply = plyTemp;
			break;
		}
	}

	return ply;
}




VRClient*  VRClientManager::GetFreePlayer()
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


bool VRClientManager::DeleteVRClient(int plyId)
{
	bool ret = false;
	VRClient* ply = NULL;
	boost::mutex::scoped_lock lock(mMutex);
	for (iterator i = begin(); i != end(); i++)
	{
		ply = (VRClient*)(*i);
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
