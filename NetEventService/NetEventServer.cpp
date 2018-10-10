//**************************************************************************
//
//  File......... : NetEventServer.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : implementation file of the class NetEventServer used to implementation 
//							common functions encapsulation for network communication based on
//							libevent  open source.
//  History...... : first created Han Liu Zhi 2018-09
//
//***************************************************************************

#include "NetEventServer.h"
#include "ChannelIDGenerator.h"
#include "event2/thread.h"
#include "event2/event.h"
#include "event2/event_compat.h"
#include "event2/listener.h"
#include "event2/buffer.h"
#include "Channel.h"
#include "MessageQueue.h"
#include "CommonDef.h"


#define MUL_LIBEVENT_THREAD
#define THREAD_NUMB 4


NetEventServer::NetEventServer()
{
	m_pMsgQueueAB = new MessageQueueAB();
	m_base = NULL;
	m_listener = NULL;
	m_channelID_set = NULL;
}


NetEventServer::~NetEventServer()
{
	Stop();
}


bool NetEventServer::Start(int port, int maxConnects)
{
	
	bool bRet = Init(0, maxConnects);
	if (!bRet) 
	{
		return false;
	}
	
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	if (-1 == port)
	{
		sin.sin_port = htons(10000);
	}
	else
	{
		sin.sin_port = htons(port);
	}

	//创建监听对象，开始接受连接
	m_listener = evconnlistener_new_bind(m_base, listener_cb, (void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

	if (!m_listener)
	{
		LOG(error, "不能建立监听！");
		return false;
	}

	if (-1 == port)
	{
		port = ntohs(sin.sin_port);
	}


	m_dispatchThread.reset(new std::thread([this]
	{
		event_base_dispatch(m_base);

		if (WSAENOTSOCK == WSAGetLastError())
		{
			LOG(error, "无效套接字！");
			exit(0);
		}
	}));

	LOG(info, "服务器成功启动! 端口号:[%d]", port);

	return true;
}


bool NetEventServer::Stop()
{
	//结束消息派发线程
	event_base_loopbreak(m_base);
	m_dispatchThread->join();

	if (m_listener != NULL)
	{
		evconnlistener_free(m_listener);
		m_listener = NULL;
	}

	if (m_base != NULL)
	{
		event_base_free(m_base);
		m_base = NULL;
	}

	//销毁消息队列
	if (m_pMsgQueueAB)
	{
		delete m_pMsgQueueAB;
		m_pMsgQueueAB = nullptr;
	}

	//
	if (m_channelID_set)
	{
		delete m_channelID_set;
		m_channelID_set = nullptr;
	}

	//结束工作线程
	for (int i = 0; i < m_libevent_threads.size(); ++i)
	{
		if (m_libevent_threads[i] != NULL)
		{
			event_base_loopbreak(m_libevent_threads[i]->thread_base);
			event_base_free(m_libevent_threads[i]->thread_base);
			m_libevent_threads[i]->thread_base = NULL;
			m_libevent_threads[i]->spThread->join();
		}
	}
	m_libevent_threads.clear();

	//销毁用户连接列表
	for (int i = 0; i < m_Channels.size(); ++i)
	{
		if (m_Channels[i] != NULL)
		{
			delete m_Channels[i];
		}
	}
	m_Channels.clear();

	LOG(info, "服务器关闭.");

	return true;
}


int NetEventServer::Send(int connectid, unsigned short id1, unsigned short id2, const void* data, unsigned int len)
{
	if (connectid < 0 || id1 < 0 || id2 < 0 || len < 0)
	{
		return send_stat::send_parameter_error;
	}

	int datalength = len;
	m_msgPack.header()->id1 = id1;
	m_msgPack.header()->id2 = id2;

	if (data == NULL)
	{
		datalength = 0;
	}

	if (datalength > 0)
	{
		memcpy(m_msgPack.body(), data, datalength);
	}

	m_msgPack.SetBodyLength(datalength);

	return Send(connectid, m_msgPack);

}

int NetEventServer::Send(int connectid, MessagePackage& msg)
{

	Channel* c = m_Channels[connectid];
	if (c == NULL)
	{
		return send_stat::send_target_not_exist;
	}

	return c->send_data(msg.data(), msg.GetPackageLength());
}

void NetEventServer::Close(int connectid)
{

	Channel* c = m_Channels[connectid];
	if (c != NULL)
	{
		c->close();
	}
}


MessageQueue& NetEventServer::GetMsgQueue()
{
	return m_pMsgQueueAB->Swap();
}


bool NetEventServer::Init(int id_begin, int id_counts)
{
	DWORD ret;
	WSADATA wsaData;

	if ((ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		LOG(error, "WSAStartup failed with error %d", ret);
		exit(1);
	}

	m_channelID_set = new ChannelIDGenerator();
	m_channelID_set->init(id_begin, id_counts);

	m_Channels.resize(m_channelID_set->getSize());

	//event支持windows下线程的函数
	int hr = evthread_use_windows_threads();


	//event_enable_debug_mode();

	m_base = event_base_new();
	if (NULL == m_base)
	{
		LOG(error, "初始化Libevent失败！");
		return false;
	}
#ifdef MUL_LIBEVENT_THREAD
	m_last_thread = -1; //注意初始化为-1
	//初始化线程
	InitWorkerThreads(THREAD_NUMB);
#endif
	return true;
}

bool NetEventServer::InitWorkerThreads(int thread_numb)
{
	m_libevent_threads.resize(thread_numb);

	for (int i = 0; i < thread_numb; ++i)
	{

		WorkerThread* plt = new WorkerThread();
#ifdef WIN32
		//创建互相通信的两个socket
		evutil_socket_t fds[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
		{
			printf( "创建socketpair失败！");
			return false;
		}
		//设置成无阻赛的socket
		evutil_make_socket_nonblocking(fds[0]);
		evutil_make_socket_nonblocking(fds[1]);
#else
		int fds[2];
		if (pipe(fds)) {
			printf("Can't create notify pipe");
			exit(1);
		}
#endif
		plt->notfiy_recv_fd = fds[0];
		plt->notfiy_send_fd = fds[1];


		SetupWorkerThread(plt);

		m_libevent_threads[i] = plt;
	}

	//创建并启动工作线程
	for (int i = 0; i < thread_numb; ++i)
	{
		m_libevent_threads[i]->spThread.reset(new std::thread([]
		(void* arg)
		{
			auto me = (WorkerThread*)arg;
			event_base_loop(me->thread_base, 0);
		}, m_libevent_threads[i]));
	}
	return true;
}

//注册通知事件及其回调函数
void NetEventServer::SetupWorkerThread(WorkerThread * pLibeventThread)
{
	auto plt = pLibeventThread;
	
	//创建属于每个线程的event_base，
	plt->thread_base = event_base_new(); 
	plt->that = this;

	int ret = event_assign(&plt->notify_event, plt->thread_base, plt->notfiy_recv_fd, EV_READ | EV_PERSIST, notify_cb, plt);

	ret  = event_add(&plt->notify_event, 0); 
}

//管道通知事件回调函数
void NetEventServer::notify_cb(evutil_socket_t fd, short which, void *args)
{

	WorkerThread * pLibeventThread = (WorkerThread *)args;

	//首先将socketpair的1个字节通知信号读出
	//在水平触发模式下如果不处理该事件，则会循环通知，直到事件被处理
	char  buf[1];
	recv(fd, buf, 1, 0);

	auto plt = pLibeventThread;

	conn_queue_item  item;

	//从自己的连接队列中取出一个连接
	{
#ifdef BOOST_LOCKFREE
		while (!plt->conn_queue.pop(item))//pop一个出来
		{
#ifndef _DEBUG
			boost::this_thread::interruptible_wait(1);
#else
			Sleep(1);
#endif
			LOG(info, "通知队列空！");
		}
#else 
		std::lock_guard<std::mutex>  lck(plt->conn_mtx);
		item = plt->conn_queue.front();
		plt->conn_queue.pop();
		
#endif
	}

	//创建每个连接socket的bufferevent
	auto bev = bufferevent_socket_new(plt->thread_base, item.fd, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
	if (!bev) 
	{
		LOG(error, "创建socket出错！");
		return;
	}

	Channel* c = pLibeventThread->that->CreateChannel(bev, item.tid);
	if (NULL == c)
	{
		LOG(info, "超过服务器最大连接数！请稍后重新连接！");
		evutil_closesocket(item.fd);

		return;
	}
	
	c->SetIPAddr(item.ip); //保存IP

	/************************************************/
	LOG(info, "[%s]连接服务器成功！", item.ip.c_str());
		
	MessagePackage msgPack;
	msgPack.header()->id1 = link_stat::link_connected;
	msgPack.header()->id2 = 0;
	msgPack.SetLinkID(c->GetChannelID()); //服务器用户需要进行用户管理用
	msgPack.SetBodyLength(0);

	plt->that->GetMessageQueueAB()->Push(msgPack);
	/************************************************/

	bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, c);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

}


void NetEventServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{

	NetEventServer * pEventServer = (NetEventServer *)user_data;

#ifdef MUL_LIBEVENT_THREAD
	// 轮循,选择工作线程
	int cur_thread = (pEventServer->m_last_thread + 1) % THREAD_NUMB; 
	pEventServer->m_last_thread = cur_thread;


	struct sockaddr_in* addr = (struct sockaddr_in *)sa;

	conn_queue_item item;
	item.fd = fd;
	item.tid = cur_thread;
	item.ip = inet_ntoa(addr->sin_addr);

	evutil_make_socket_nonblocking(item.fd);

	auto  plt = pEventServer->m_libevent_threads[cur_thread];
	{
		//向线程的队列中放入一个连接的socketfd
#ifdef BOOST_LOCKFREE
		while (!plt->conn_queue.push(item))
		{
#ifndef _DEBUG
			boost::this_thread::interruptible_wait(1);
#else
			Sleep(1);
#endif
			LOG(error, "连接队列超过1000连接数！");
		}
#else
		std::lock_guard<std::mutex> lock(plt->conn_mtx);
		plt->conn_queue.push(item);
#endif
	}

	//激活所选线程的通知事件回调函数notify_cb
	send(plt->notfiy_send_fd, "c", 1, 0);

#else
	auto base = evconnlistener_get_base(listener);

	auto bev = bufferevent_socket_new(base, fd, BEV_OPT_THREADSAFE);//|BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		printf( "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}

	auto c2 = CreateChannel(bev);

	bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, c2);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

#endif
}


Channel* NetEventServer::CreateChannel(bufferevent *bev, int tid)
{
	int cid = m_channelID_set->getId();

	if (cid == -1)
	{
		LOG(info, "最大用户数已满！");
		return NULL;
	}

	/************************************************/
	if ((NULL != m_Channels[cid]) && m_Channels[cid]->IsUsed())
	{
		MessagePackage msgPack;
		msgPack.header()->id1 = link_stat::link_error_channel_is_exist;
		msgPack.header()->id2 = 0;
		msgPack.SetBodyLength(0);

		m_pMsgQueueAB->Push(msgPack);

		return NULL;
	}
	/************************************************/

	evutil_socket_t fd =  bufferevent_getfd(bev);

	Channel* c = new Channel(bev);
	c->SetNetEventServer(this);
	c->SetChannelID(cid);
	c->SetTID(tid);
	c->SetFD(fd);
	c->SeMsgQueueAB(this->GetMessageQueueAB());
	m_Channels[cid]  =  c;

	return c;
}


void NetEventServer::conn_readcb(struct bufferevent *bev, void *arg)
{

	Channel* c = (Channel*)arg;
	c->SetBufferEvent(bev);

	c->handle_read();						//数据流到channel中
}

void NetEventServer::conn_eventcb(struct bufferevent *bev, short what, void *arg)
{
	struct evbuffer *output = bufferevent_get_output(bev);
	size_t remain = evbuffer_get_length(output);

	if (what & BEV_EVENT_TIMEOUT)
	{
		LOG(info,"连接超时！");  //if bufferevent_set_timeouts() called.
	}
	else if (what & BEV_EVENT_EOF)
	{
		LOG(info, "客户端连接关闭！剩余消息数： %d", remain);
	}
	else if (what & BEV_EVENT_ERROR)
	{
		LOG(info, "客户端发生不明原因错误！ 剩余消息数： %d", remain);
	}

	Channel* c = (Channel*)arg;
	if (NULL != c)
	{
		c->close();
	}
}
