//**************************************************************************
//  File......... : MessagePackage.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : head file of the MessagePackage struct.
//
//  History...... : first created by Liu Zhi 2018-09
//
//***************************************************************************


#pragma once

typedef int LinkID;

#pragma pack(push, 1)
struct MessagePackage
{
public:
	struct Header
	{
		int						length;
		unsigned short	id1;
		unsigned short	id2;
	};

public:
	enum { header_length = sizeof(Header) };
	enum { max_body_length = 32 * 1024 };

	Header* header()  { return (Header*)data_; }
	char* body() { return data_ + header_length; }
	char* data() { return data_; }
	int GetLinkID() const { return m_linkid; }
	void SetLinkID(int linkID) { m_linkid = linkID; }
	int	 GetPackageLength() const { return header_length + GetBodyLength(); }
	
	int	 GetBodyLength() const { return ((Header*)data_)->length; }
	void SetBodyLength(int len) { header()->length = len; }

private:
	LinkID	m_linkid;
	char	data_[header_length + max_body_length];

};
#pragma pack(pop)