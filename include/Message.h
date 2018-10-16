//**************************************************************************
//  File......... : Message.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Head file of the MessagePackage struct.
//
//  History...... : First created by Liu Zhi 2018-09
//
//***************************************************************************
#pragma once
#include <vector>


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
	enum { max_body_length = 8 * 1024 };

	Header* header() { return (Header*)m_buffer; }
	char* body() { return m_buffer + header_length; }
	char* data() { return m_buffer; }
	
	int GetLinkID() const { return m_linkid; }
	void SetLinkID(int linkID) { m_linkid = linkID; }

	int	 GetPackageLength() const { return header_length + GetBodyLength(); }
	int	 GetBodyLength() const { return ((Header*)m_buffer)->length; }


	bool WriteHeader(short msgID, short cmdID)
	{
		if (msgID <= 0 || cmdID < 0)
		{
			return false;
		}
		Header* pHeader = header();

		pHeader->id1 = msgID;
		pHeader->id2 = cmdID;
		pHeader->length = 0;

		return true;
	}

	bool WriteBody(void* in, int len)
	{
		if (len < 0 || len > max_body_length)
		{
			return false;
		}

		if (NULL != in)
		{
			unsigned char* pData = (unsigned char*)in;

			memcpy(body(), pData, len);
			header()->length = len;
		}
		return true;
	}

private:
	LinkID	m_linkid;
	char		m_buffer[header_length + max_body_length];
};
#pragma pack(pop)