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

	//MessagePackage() { m_Buffer.reserve(64); }

	Header* header() { return (Header*)data_; }
	char* body() { return data_ + header_length; }
	char* data() { return data_; }
	int GetLinkID() const { return m_linkid; }
	void SetLinkID(int linkID) { m_linkid = linkID; }
	int	 GetPackageLength() const { return header_length + GetBodyLength(); }

	int	 GetBodyLength() const { return ((Header*)data_)->length; }
	//void SetBodyLength(int len) { header()->length = len; }


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
	char	data_[header_length + max_body_length];
	
	//std::vector<unsigned char> m_Buffer;
	//char* data_ = static_cast<char*>(&m_Buffer);

};


//struct MessagePackage
//{
//public:
//	struct Header
//	{
//		int						length;
//		unsigned short	id1;
//		unsigned short	id2;
//	};
//
//public:
//	enum { header_length = sizeof(Header) };
//	enum { max_body_length = 32 * 1024 };
//
//	//MessagePackage() { m_Buffer.resize(64); }
//
//	Header* header() { return (Header*)(char*)&m_Buffer[0]; }
//	char* body() { return (char*)&m_Buffer[header_length]; }
//	char* data() { return (char*)&m_Buffer[0]; }
//	int GetLinkID() const { return m_linkid; }
//	void SetLinkID(int linkID) { m_linkid = linkID; }
//	int	 GetPackageLength() const { return m_Buffer.size(); }
//
//	int	 GetBodyLength()  { return header()->length; }
//	//void SetBodyLength(int len) { header()->length = len; }
//
//	bool WriteHeader(short msgID, short cmdID)
//	{
//		if (msgID <= 0 || cmdID < 0)
//		{
//			return false;
//		}
//		Header* pHeader = header();
//
//		pHeader->id1 = cmdID;
//		pHeader->id2 = cmdID;
//		pHeader->length = 0;
//
//		return true;
//	}
//
//	bool WriteBody(void* in, int len)
//	{
//		if (len < 0)
//		{
//			return false;
//		}
//		
//		unsigned char* pData = (unsigned char*)in;
//		if (NULL != pData)
//		{
//			const unsigned int end = header_length + len;
//			if (m_Buffer.size() < end)
//			{
//				m_Buffer.resize(end);
//			}
//
//			memcpy(&m_Buffer[0] + header_length, pData, len);
//
//			header()->length = len;
//		}
//		return true;
//	}
//
//
//private:
//	LinkID	m_linkid;
//	std::vector<unsigned char> m_Buffer;
//
//};


#pragma pack(pop)