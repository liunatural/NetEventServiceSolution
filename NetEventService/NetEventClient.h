#pragma once
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "NetEventService.h"
#include "DataStream.h"


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
	static void Handle_read(NetEventClient* pc);
	void read_pack();


private:
	event_base *m_pBase;
	struct bufferevent* m_bev;
	MessageQueueAB *m_pMsgQueueAB;
	MessagePackage	 m_msgPack;
	DataStream	m_readStream;


	//char* m_pszMsg;
	//struct event *m_evtimeout;

	struct timeval m_timeout;

};

