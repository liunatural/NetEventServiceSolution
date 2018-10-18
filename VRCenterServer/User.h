#pragma once

typedef int LinkID;
class User
{
public:
	User(LinkID linkID);
	virtual ~User();
	LinkID& GetLinkID() { return mLinkID; };

public:
	int		mUserID;
	int		mUserType;				
private:
	LinkID mLinkID;
	bool		mFlvSeqHeaderFlag;
};
