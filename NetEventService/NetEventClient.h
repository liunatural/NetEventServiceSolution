#pragma once
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "NetEventService.h"
#include "DataStream.h"
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

	DataStream& GetDataStream() {return m_readStream;}


private:
	static void readcb(struct bufferevent* bev, void* arg);
	static void eventcb(struct bufferevent *bev, short event, void *arg);
	void read_pack();

	static void signal_cb(evutil_socket_t sig, short events, void *arg);


private:
	event_base *m_pBase;
	struct bufferevent* m_bev;
	MessageQueueAB *m_pMsgQueueAB;
	MessagePackage	 m_msgPack;
	DataStream	m_readStream;


	std::shared_ptr<std::thread> m_thread;

	struct timeval m_timeout;
	struct event* m_pEvstop;


};

