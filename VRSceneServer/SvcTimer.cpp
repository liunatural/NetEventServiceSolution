#include "SvcTimer.h"
#include "protocol.h"

//���뼶��ʱ���� ÿ��30���봥��һ�Σ� ������ҵ�λ����Ϣ
SvcTimer::SvcTimer(io_service& ios, PlayerManager*& playerMgr)
	: mTimer(ios, boost::posix_time::milliseconds(30)), mPlayMgr(playerMgr)
{
	mTimer.async_wait(boost::bind(&SvcTimer::handler, this));
}

SvcTimer::~SvcTimer()
{
}

//void SvcTimer::handler()
//{
//	//Ϊ�˷�ֹ�ڷ������ݰ�ʱ������˳������������� ������ط�����
//	boost::mutex::scoped_lock lock(mPlayMgr->mMutex);		
//
//	memset(buffer, 0, sizeof(buffer));
//	char*p = buffer;
//	int count = 0;
//
//	for (PlayerManager::iterator i = mPlayMgr->begin(); i != mPlayMgr->end(); i++)
//	{
//		if ((sizeof(buffer) - count * sizeof(TransformInfo)) < sizeof(TransformInfo))
//		{
//			break;
//		}
//		Player* ply = (Player*)(*i);
//		if (NULL != ply && ply->transInfo.update)
//		{
//			memcpy(p, (const void*)&ply->transInfo, sizeof(TransformInfo));
//			ply->transInfo.update = false;
//
//			p += sizeof(TransformInfo);
//			count++;
//		}
//	}
//
//	if(count > 0)
//	{ 
//		for (PlayerManager::iterator j = mPlayMgr->begin(); j != mPlayMgr->end(); j++)
//		{
//			Player* ply1 = (Player*)(*j);
//			if (NULL != ply1)
//			{
//				mPlayMgr->SendCmd(ply1->GetLinkID(), ID_User_Transform, 0, &buffer, sizeof(TransformInfo)* count);
//			}
//		}
//
//		//if (mPlayMgr->GetUserVisibilityExternal())
//		//{
//		//	mPlayMgr->GetCenterSvrConnection()->Send(ID_Global_Transform, 0, (char*)&buffer, sizeof(TransformInfo)* count);
//		//}
//
//	}
//
//	mTimer.expires_at(mTimer.expires_at() + boost::posix_time::milliseconds(30));
//	mTimer.async_wait(boost::bind(&SvcTimer::handler, this));
//}


void SvcTimer::handler()
{
		//Ϊ�˷�ֹ�ڷ������ݰ�ʱ������˳������������� ������ط�����
		boost::mutex::scoped_lock lock(mPlayMgr->mMutex);	

		SendTransformDataByUserType(VIP);
		SendTransformDataByUserType(ExternalVIP);

		mTimer.expires_at(mTimer.expires_at() + boost::posix_time::milliseconds(30));
		mTimer.async_wait(boost::bind(&SvcTimer::handler, this));

}


void SvcTimer::SendTransformDataByUserType(UserType usrType)
{
	memset(buffer, 0, sizeof(buffer));
	char*p = buffer;
	int count = 0;

	for (PlayerManager::iterator i = mPlayMgr->begin(); i != mPlayMgr->end(); i++)
	{
		if ((sizeof(buffer) - count * sizeof(TransformInfo)) < sizeof(TransformInfo))
		{
			break;
		}
		Player* ply = (Player*)(*i);
		if (NULL != ply && ply->transInfo.update && (ply->GetUserType() == usrType))
		{
			memcpy(p, (const void*)&ply->transInfo, sizeof(TransformInfo));
			ply->transInfo.update = false;

			p += sizeof(TransformInfo);
			count++;
		}
	}

	if (count > 0)
	{

		int len = sizeof(TransformInfo)* count;

		for (PlayerManager::iterator j = mPlayMgr->begin(); j != mPlayMgr->end(); j++)
		{
			Player* ply1 = (Player*)(*j);
			if (NULL != ply1)
			{
				mPlayMgr->SendCmd(ply1->GetLinkID(), ID_User_Transform, 0, &buffer, len);
			}
		}

		if (usrType == ExternalVIP)
		{
			NetEvtClient* pCenterSvrClient = mPlayMgr->GetCenterSvrConnection();

			if (pCenterSvrClient)
			{
				pCenterSvrClient->Send(ID_Global_Transform, 0, (char*)&buffer, len);
			}

		}
	}

}

