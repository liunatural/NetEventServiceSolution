#pragma once
#include "protocol.h"
#include  "Message.h"
#include "string.h"

class VRClient
{
public:
	VRClient(LinkID linkID);
	virtual ~VRClient();
	LinkID& GetLinkID() { return mLinkID; };

	//场景服务器ID
	char* GetSceneServerID() { return mProfileInfo.SceneServerID; }
	void SetSceneCntrlID(char* pSceneSvrID) { memcpy(mProfileInfo.SceneServerID, pSceneSvrID, SCENE_SERVER_ID_LENGTH); }


	void SetSeatNumber(int seatNum) { mSeatNumber = mProfileInfo.mSeatNumber = seatNum; }
	int GetSeatNumber() { return mSeatNumber; }

	void SetUserID(char* userid, int len);


	void UpdateProfileInfo(ProfileInfo* profileInfo) { mProfileInfo = *profileInfo; }

	void SetUserType(int userType) {	mUserType = userType;}
	int GetUserType() { return mUserType; }


	void SetUserState(UserState userState) { mUserState = userState; }
	UserState GetUserState() { return mUserState; }

public:
	int		mPlyID;
	bool	bBoundUser;
	char mUserID[USER_ID_LENGTH + 1];

	UserState mUserState;
	ProfileInfo mProfileInfo;
	TransformInfo transInfo;

private:
	int mSeatNumber;
	LinkID mLinkID;
	bool		mFlvSeqHeaderFlag;
	int		mUserType;			//1: VIP客户端， 2: 胶囊客户端




};