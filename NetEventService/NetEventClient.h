//**************************************************************************
//  File......... : NetEventClient.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : head file of the class NetEventClient used as encapsulation to common 
//							functions in libevent open source library for network communication.
//  History...... : first created by Liu Zhi 2018-09
//
//***************************************************************************


#pragma once
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "NetEventService.h"
#include "DynamicBuffer.h"
#include <thread>


class MessageQueueAB;
class NetEventClient : public NetEvtClient
{
public:
	NetEventClient();
	virtual ~NetEventClient();

	int  Connect(const char* ip, const char* port);
	void Start();
	void	Disconnect();
	MsgQueue&	GetMsgQueue();
	int Send(MessagePackage& msg);
	int	 Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len);

	DynamicBuffer& GetDataStream() {return m_readStream;}

	void SetEventCallback(fn_on_event_cb fncb) { m_fncb = fncb; }

private:
	static void readcb(struct bufferevent* bev, void* arg);
	static void eventcb(struct bufferevent *bev, short event, void *arg);
	void read_pack();
	void clear();
	static void signal_cb(evutil_socket_t sig, short events, void *arg);


private:
	event_base *m_pBase;
	struct bufferevent* m_bev;
	MessageQueueAB *m_pMsgQueueAB;
	MessagePackage	 m_msgPack;
	DynamicBuffer	m_readStream;


	std::shared_ptr<std::thread> m_thread;

	struct timeval m_timeout;
	struct event* m_pEvstop;

	fn_on_event_cb m_fncb;

};

