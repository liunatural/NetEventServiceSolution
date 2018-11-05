//**************************************************************************
//  File......... : NetEventServer.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Implementation file of the class NetEventServer used as encapsulation to common 
//							functions in libevent open source library for network communication.
//  History...... : First created by Liu Zhi 2018-09
//
//***************************************************************************
#include "NetEventServer.h"
#include "event2/thread.h"
#include "event2/event.h"
#include "event2/event_compat.h"
#include "event2/listener.h"
#include "event2/buffer.h"
#include "Channel.h"
#include "ChannelManager.h"
#include "MessageQueue.h"
#include "protocol.h"
#include <thread>

NetEventServer::NetEventServer()
{
	m_pMsgQueueAB = new MessageQueueAB();
	m_base = NULL;
	m_listener = NULL;
	m_channelMgr = NULL;

	m_thread_num = 1;

	m_pSendMsgQueue = new CommonMsgQueue<MessagePackage>(MAX_SEND_MSGQ_LEN);

	m_bStopThread = false;
}


NetEventServer::~NetEventServer()
{
	Stop();
}


bool NetEventServer::Start(int port, int maxConnects)
{
	//并发线程数
	m_thread_num = std::thread::hardware_concurrency() / 2;

	bool bRet = Init(0, maxConnects);
	if (!bRet) 
	{
		return false;
	}
	
	//启动接收线程
	bRet = StartReceiverThreads(m_thread_num);
	if (!bRet)
	{
		return false;
	}

	//启动发送线程
	StartSenderThreads(m_thread_num);

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
			return false;
		}
	}));

	LOG(info, "服务开始运行! 端口号:[%d]; 线程数:[%d]; 最大连接数[%d]", port, m_thread_num, maxConnects);

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

	//销毁消息接收队列
	if (m_pMsgQueueAB)
	{
		delete m_pMsgQueueAB;
		m_pMsgQueueAB = nullptr;
	}

	//
	if (m_channelMgr)
	{
		delete m_channelMgr;
		m_channelMgr = nullptr;
	}

	//结束接收线程
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


	//结束发送线程
	m_bStopThread = true;
	for (int i = 0; i < m_send_threads.size(); ++i)
	{
		m_send_threads[i].join();
	}


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
		return send_parameter_error;
	}

	m_msgPack.WriteHeader(id1, id2);
	m_msgPack.WriteBody((void*)data, len);

	return Send(connectid, m_msgPack);
}

int NetEventServer::Send(int connectid, MessagePackage& msg)
{

	msg.SetLinkID(connectid);
	m_pSendMsgQueue->put(msg);

	return true;
}


int NetEventServer::DoSend(MessagePackage& msg)
{
	std::lock_guard<std::mutex> lock(m_channel_mtx);
	
	int connectid = msg.GetLinkID();

	Channel* c = m_Channels[connectid];
	if (c == NULL)
	{
		return send_target_not_exist;
	}

	return c->SendData(msg.data(), msg.GetPackageLength());
}


void NetEventServer::sender_thread_task(NetEventServer *pNetEvtSvr)
{
	if (!pNetEvtSvr)
	{
		return;
	}

	MessagePackage msgpacK;
	bool bIsEmpty = false;

	while (true)
	{
		bool bStopFlag = pNetEvtSvr->GetStopSenderThreadFlag();
		if (bStopFlag == true)
		{
			break;
		}

		//pNetEvtSvr->GetSendMsgQueue()->get(msgpacK);
		//pNetEvtSvr->DoSend(msgpacK);

		pNetEvtSvr->GetSendMsgQueue()->get(msgpacK, bIsEmpty);
		if (bIsEmpty)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
		}
		else
		{
			
			pNetEvtSvr->DoSend(msgpacK);
		}
	}

	LOG(error, "发送线程结束并退出.");

}

void NetEventServer::StartSenderThreads(int thread_numb)
{
	m_send_threads.resize(thread_numb);
	for (int i = 0; i < thread_numb; i++)
	{
		m_send_threads[i]  = std::thread(sender_thread_task, this);
	}
}


void NetEventServer::Close(int connectid)
{
	std::lock_guard<std::mutex> lock(m_channel_mtx);

	Channel* c = m_Channels[connectid];
	if (c != NULL)
	{
		c->CloseSocket();
		delete c;
		m_Channels[connectid] = NULL;

	}
}


int NetEventServer::GetOnlineAmount()
{
	return m_channelMgr->TotalIDs() - m_channelMgr->TotalFreeIDs();
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
		LOG(error, "WSAStartup失败！错误ID:%d", ret);
		return false;
	}

	m_channelMgr = new ChannelManager();
	m_channelMgr->Init(id_begin, id_counts);

	m_Channels.resize(m_channelMgr->TotalIDs());

	//event支持windows下线程的函数
	int hr = evthread_use_windows_threads();

	m_base = event_base_new();
	if (NULL == m_base)
	{
		LOG(error, "初始化event_base失败.");
		return false;
	}

	SetEventConfig();

	m_last_thread = -1; 

	return true;
}

bool NetEventServer::StartReceiverThreads(int thread_numb)
{

	m_libevent_threads.resize(thread_numb);

	for (int i = 0; i < thread_numb; ++i)
	{

		ReceiverThread* plt = new ReceiverThread();

		//创建互相通信的两个socket
		evutil_socket_t fds[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
		{
			LOG(error, "创建socketpair失败！");
			return false;
		}
		//设置成无阻赛的socket
		evutil_make_socket_nonblocking(fds[0]);
		evutil_make_socket_nonblocking(fds[1]);

		plt->notfiy_recv_fd = fds[0];
		plt->notfiy_send_fd = fds[1];


		bool bRet = SetupReceiverThread(plt);
		if (!bRet)
		{
			LOG(error, "SetupReceiverThread:注册通知事件失败.");
			return false;
		}

		m_libevent_threads[i] = plt;
	}

	//创建并启动工作线程
	for (int i = 0; i < thread_numb; ++i)
	{
		m_libevent_threads[i]->spThread.reset(new std::thread([]
		(void* arg)
		{
			auto me = (ReceiverThread*)arg;
			event_base_loop(me->thread_base, 0);
		}, m_libevent_threads[i]));
	}

	return true;
}

//注册通知事件及其回调函数
bool NetEventServer::SetupReceiverThread(ReceiverThread * pLibeventThread)
{
	auto plt = pLibeventThread;
	
	//创建属于每个线程的event_base，
	plt->thread_base = event_base_new_with_config(m_ec);
	//plt->thread_base = event_base_new(); 
	plt->that = this;

	int ret = event_assign(&plt->notify_event, plt->thread_base, plt->notfiy_recv_fd, EV_READ | EV_PERSIST, notify_cb, plt);
	if (ret == SUCCESS)
	{
		ret = event_add(&plt->notify_event, 0);
	}

	bool bRet = (ret == SUCCESS);
	
	return bRet;
}


//管道通知事件回调函数
void NetEventServer::notify_cb(evutil_socket_t fd, short which, void *args)
{

	ReceiverThread * pLibeventThread = (ReceiverThread *)args;

	//首先将socketpair的1个字节通知信号读出
	//在水平触发模式下如果不处理该事件，则会循环通知，直到事件被处理
	char  buf[1];
	recv(fd, buf, 1, 0);

	auto plt = pLibeventThread;

	conn_queue_item  item;

	//从自己的连接队列中取出一个连接
	std::lock_guard<std::mutex>  lck(plt->conn_mtx);
	item = plt->conn_queue.front();
	plt->conn_queue.pop();


	//创建每个连接socket的bufferevent
	auto bev = bufferevent_socket_new(plt->thread_base, item.fd, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);
	if (!bev) 
	{
		LOG(error, "创建socket出错！");
		return;
	}

	Channel* c = pLibeventThread->that->CreateChannel(bev, item);
	if (NULL != c)
	{
		c->SetIPAddr(item.ip); //保存IP
		LOG(info, "[%s]登录服务器！当前在线人数: [%d]", item.ip.c_str(), plt->that->GetOnlineAmount());

		MessagePackage msgPack;
		msgPack.WriteHeader(link_connected, 0);
		msgPack.SetLinkID(c->GetChannelID());

		plt->that->GetMessageQueueAB()->Push(msgPack);						//返回消息包给应用层，进行用户管理
		//send(item.fd, msgPack.data(), msgPack.GetPackageLength(), 0);		//同时向用户发送连接成功的消息

		bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, c);
		bufferevent_enable(bev, EV_READ | EV_WRITE);

	}
}


void NetEventServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data)
{

	NetEventServer * pEventServer = (NetEventServer *)user_data;

	// 轮循,选择工作线程
	int cur_thread = (pEventServer->m_last_thread + 1) % pEventServer->m_thread_num;
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
		std::lock_guard<std::mutex> lock(plt->conn_mtx);
		plt->conn_queue.push(item);
	}

	//激活所选线程的通知事件回调函数notify_cb
	send(plt->notfiy_send_fd, "c", 1, 0);
}


Channel* NetEventServer::CreateChannel(bufferevent *bev, conn_queue_item& connItem)
{
	int cid = m_channelMgr->GetFreeID();
	MessagePackage msgPack;

	if (cid == -1)
	{
		LOG(info, "[%s]尝试连接服务器失败！超过服务器最大连接数！", connItem.ip.c_str());
		msgPack.WriteHeader(link_error_exceed_max_connects, 0);

		send(connItem.fd, msgPack.data(), msgPack.GetPackageLength(), 0);

		Sleep(10);
		evutil_closesocket(connItem.fd);

		return NULL;
	}

	//这种情况应该不会发生，m_channelMgr->GetFreeID()已经加锁
	if ((NULL != m_Channels[cid]) && m_Channels[cid]->IsUsed()) 
	{
		LOG(error, "服务器分配了一个正在使用中的连接ID[%d]！", cid);
		
		//报告给上层应用
		msgPack.WriteHeader(link_error_channel_is_exist, 0);
		m_pMsgQueueAB->Push(msgPack);

		//断开该连接
		evutil_closesocket(connItem.fd);

		return NULL;
	}

	evutil_socket_t fd =  bufferevent_getfd(bev);

	Channel* c = new Channel(bev);
	c->SetNetEventServer(this);
	c->SetChannelID(cid);
	c->SetTID(connItem.tid);
	c->SetFD(fd);
	c->SeMsgQueueAB(this->GetMessageQueueAB());
	m_Channels[cid]  =  c;

	return c;
}


bool NetEventServer::SetEventConfig()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	int cpu_num = si.dwNumberOfProcessors;

	m_ec = event_config_new();
	//event_config_set_flag(m_ec, EVENT_BASE_FLAG_STARTUP_IOCP);
	//event_config_require_features(m_ec, EV_FEATURE_ET);

	event_config_set_num_cpus_hint(m_ec, cpu_num);

	return true;
}


void NetEventServer::conn_readcb(struct bufferevent *bev, void *arg)
{

	Channel* c = (Channel*)arg;
	c->SetBufferEvent(bev);

	c->DoRead();			//数据流到channel中
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
		LOG(info, "客户端关闭连接！剩余消息数： %d", remain);
	}
	else if (what & BEV_EVENT_ERROR)
	{
		LOG(info, "客户端退出！ 剩余消息数： %d", remain);
	}

	Channel* c = (Channel*)arg;
	if (NULL != c)
	{
		NetEventServer* pNetEventSvr = c->GetNetEventServer();
		int cid = c->GetChannelID();
		string userIP = c->GetIPAddr();

		pNetEventSvr->Close(cid);

		LOG(info, "[%s]用户退出！ 剩余在线人数: [%d]", userIP.c_str(), pNetEventSvr->GetOnlineAmount());
	}

}
