//**************************************************************************
//  File......... : NetEventClient.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : implementation file of the class NetEventClient used as encapsulation to common 
//							functions in libevent open source library for network communication.
//  History...... : first created by Liu Zhi 2018-09
//
//**************************************************************************

#include "NetEventClient.h"
#include "MessageQueue.h"
#include "protocol.h"
#include <signal.h>

NetEventClient::NetEventClient() 
{
	m_pMsgQueueAB = new MessageQueueAB();
}


NetEventClient::~NetEventClient()
{
	m_thread->join();
}

int NetEventClient::Connect(const char* ip, const char* port)
{
	WSADATA wsaData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		LOG(error, "WSAStartup错误： %d\n", Ret);
		return -1;
	}


	m_pBase = event_base_new();
	if (!m_pBase)
	{
		LOG(error, "Couldn't open event base!");
		return -1;
	}


	m_pEvstop = evsignal_new(m_pBase, SIGINT, signal_cb, m_pBase);
	evsignal_add(m_pEvstop, NULL);


	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	int iPort = atoi(port);
	server_addr.sin_port = htons(iPort);
	server_addr.sin_addr.s_addr = inet_addr(ip);


	m_bev = bufferevent_socket_new(m_pBase, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!m_bev)
	{
		LOG(error, "Error constructing bufferevent!");
		return -1;
	}


	bufferevent_setcb(m_bev, readcb, NULL, eventcb, this);
	bufferevent_enable(m_bev, EV_READ | EV_WRITE | EV_PERSIST);

	if (bufferevent_socket_connect(m_bev, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		LOG(error, "Error starting connection!");
		bufferevent_free(m_bev);
		return -1;
	}

	return 0;
}


void NetEventClient::Start()
{
	m_thread.reset(new std::thread([this]
	{
		event_base_dispatch(m_pBase);

	}));
}

void NetEventClient::Disconnect()
{
	event_base_loopexit(m_pBase, NULL);
	//event_base_loopbreak(m_pBase);

	if (NULL != m_bev)
	{
		bufferevent_free(m_bev);
	}

	if (NULL != m_pBase)
	{
		event_base_free(m_pBase);
	}

}

MsgQueue& NetEventClient::GetMsgQueue()
{
	return m_pMsgQueueAB->Swap();
}

int NetEventClient::Send(MessagePackage& msg)
{
	evutil_socket_t fd = bufferevent_getfd(m_bev);
	int ret = send(fd, msg.data(), msg.GetPackageLength(), 0);

	return ret;
}

int NetEventClient::Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len)
{

	if (id1 < 0 || id2 < 0 || len < 0)
	{
		return send_parameter_error;
	}

	m_msgPack.WriteHeader(id1, id2);
	m_msgPack.WriteBody((void*)data, len);

	return Send(m_msgPack);
}

void NetEventClient::readcb(struct bufferevent* bev, void* arg)
{

	NetEventClient* pNetEventClient = (NetEventClient*)arg;

	char data[1024 * 4] = { 0 };
	int nbytes = 0;
	int TotalBytes = 0;

	if (bev == NULL)
	{
		return;
	}

	int len = (int)bufferevent_read(bev, data, sizeof(data));
	pNetEventClient->GetDataStream().Push(data, len);

	pNetEventClient->read_pack();

}


void NetEventClient::read_pack()
{
	while (m_readStream.Size() >= 8)
	{

		int len = *(int*)m_readStream.Peek();
		if (m_readStream.Size() < len + 8)
			break;

		char* data = m_readStream.Peek();
		
		memcpy(m_msgPack.data(), data, len + 8);

		m_pMsgQueueAB->Push(m_msgPack);

		m_readStream.Pop(len + 8);
	}

}

void NetEventClient::signal_cb(evutil_socket_t sig, short events, void * arg)
{

	struct event_base *base = (event_base *)arg;
	LOG(error, "exception: interrupt, stop now!\n");

	event_base_loopexit(base, NULL);

}

void NetEventClient::eventcb(struct bufferevent *bev, short event, void *arg)
{

	if (event & BEV_EVENT_EOF)
	{
		LOG(info, "服务器端断开连接！");
	}
	else if (event & BEV_EVENT_ERROR)
	{
		LOG(error, "服务器端发生错误！");
	}
	else if (event & BEV_EVENT_CONNECTED)
	{
		LOG(info, "成功连接服务器！");
		evutil_socket_t fd = bufferevent_getfd(bev);
		evutil_make_socket_nonblocking(fd);
	}

}
