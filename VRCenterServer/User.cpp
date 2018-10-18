#include "User.h"
#include "protocol.h"

User::User(LinkID linkID)
{

	mUserType = SceneServer;	
	mLinkID = linkID;
	mUserID = linkID;
}


User::~User()
{
}

