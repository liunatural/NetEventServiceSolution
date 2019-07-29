//**************************************************************************
//  File......... : Channel.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : head file of the class Channel used as user messages processing 
//						and user connection data managements.
//
//  History...... : first created by Liu Zhi 2018-09
//**************************************************************************

#pragma once
#include <event2/bufferevent.h>
#include "DynamicBuffer.h"
#include <mutex>
#include <chrono>

class NetEventServer;
class MessageQueueAB;

class Channel
{
public:
	Channel(struct bufferevent *bev);
	virtual ~Channel();

	int GetChannelID() { return m_channelID; };
	void  SetChannelID(int id) { m_channelID = id; };

	int GetFD() { return m_fd; };
	void SetFD(int fd) { m_fd = fd; };

	int GetTID() { return m_tid; };
	void  SetTID(int tid) { m_tid = tid; };

	MessageQueueAB* GetMsgQueueAB();
	void  SetMsgQueueAB(MessageQueueAB* pMsgQAB);

	void SetNetEventServer(NetEventServer* evtServer) { m_pNetEvtSvr = evtServer; };
	NetEventServer* GetNetEventServer();
	void SetBufferEvent(bufferevent* bev) { m_bev = bev; };

	bool IsUsed() { return m_bUsedFlag; };
	void SetUsedFlag(bool bflag) { m_bUsedFlag = bflag; };

	std::string& GetIPAddr() { return m_ip; };
	void SetIPAddr(std::string ip) { m_ip = ip; };

	void DoRead();
	void ReadPackage();
	void CloseSocket();
	int SendData(void* data, int len);

private:
	int								m_channelID;
	int								m_tid;	//thread ID£¨”√”⁄debug
	bool							m_bUsedFlag = false;
	evutil_socket_t			m_fd;
	std::string					m_ip;
	DynamicBuffer*			m_readBuffer;
	MessageQueueAB		*m_pMsgQAB;
	NetEventServer			*m_pNetEvtSvr;
	bufferevent				*m_bev;
};

