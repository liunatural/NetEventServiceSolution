#pragma once
#include <event2/bufferevent.h>
#include "DataStream.h"
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
	void  SeMsgQueueAB(MessageQueueAB* pMsgQAB);

	std::mutex& GetChannelMutex() { return channel_mtx; };

	void SetNetEventServer(NetEventServer* evtServer) { m_pNetEvtSvr = evtServer; };
	void SetBufferEvent(bufferevent* bev) { m_bev = bev; };

	bool IsUsed() { return m_bUsedFlag; };
	void SetUsedFlag(bool bflag) { m_bUsedFlag = bflag; };

	std::string& GetIPAddr() { return m_ip; };
	void SetIPAddr(std::string ip) { m_ip = ip; };

	void handle_read();
	void read_pack();
	void close();
	//send_stat send_data(unsigned short id1, unsigned short id2, const void* data, int len);
	send_stat send_data(void* data, int len);

private:

	std::string m_ip;
	evutil_socket_t m_fd;
	int m_channelID;
	int m_tid;	//thread ID
	bool m_bUsedFlag = false;

	MessageQueueAB *m_pMsgQAB;
	DataStream	m_readStream;
	std::mutex channel_mtx;
	NetEventServer* m_pNetEvtSvr;
	bufferevent* m_bev;
};

