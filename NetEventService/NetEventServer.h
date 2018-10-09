#pragma once
#include "NetEventService.h"
#include "MessageQueue.h"
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>

using namespace std;

class NetEventServer;
class Channel;
class Channel2;
class ChannelIDGenerator;
class MessageQueueAB;
enum send_stat;


typedef struct 
{
	evutil_socket_t fd;
	int tid;						//threadID
	std::string ip;
}conn_queue_item;

//工作线程结构体
typedef struct 
{
	NetEventServer* that;									//用作传参
	std::shared_ptr<std::thread>   spThread;		// 线程
	struct event_base * thread_base;					// 事件基
	struct event   notify_event;
	evutil_socket_t  notfiy_recv_fd;						// socketpair 接收端fd（工作线程接收通知）
	evutil_socket_t  notfiy_send_fd;						// socketpair 发送端fd（监听线程发送通知）
#ifdef BOOST_LOCKFREE
	boost::lockfree::spsc_queue<conn_queue_item, boost::lockfree::capacity<1000> > conn_queue;
#else
	std::mutex conn_mtx;									 //维护连接队列的锁
	std::queue<conn_queue_item>  conn_queue;  //conn_queue 是一个管理conn_queue_item的队列
#endif

	bool m_bStop;
}WorkerThread;



class NetEventServer : public NetEvtServer
{
public:
	NetEventServer();
	virtual ~NetEventServer();

	bool Start(int port, int maxConnects = 1000);
	bool Stop();

	MessageQueue&	 GetMsgQueue();
	int Send(int connectid, MessagePackage& msg);
	int	 Send(int connectid, unsigned short id1, unsigned short id2, const void* data, unsigned int len);
	void Close(int connectid);

	//-----------------------------------------------------------------------------------------//
	MessageQueueAB* GetMessageQueueAB() { return m_pMsgQueueAB; };
	std::vector<Channel*>* GetChannels() { return &m_Channels; };
	ChannelIDGenerator* GetChannelIDSet() { return m_channelID_set; };

private:
	static void notify_cb(evutil_socket_t fd, short which, void *args);
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data);
	static void conn_readcb(struct bufferevent *bev, void *arg);
	static void conn_eventcb(struct bufferevent *bev, short what, void *arg);

	bool Init(int start, int size);
	bool InitWorkerThreads(int thread_numb);
	void SetupWorkerThread(WorkerThread * pLibeventThread);
	Channel* CreateChannel(bufferevent *bev, int tid);

public:
	std::mutex							m_channel_mtx;

private:
	struct event_base					*m_base;
	struct evconnlistener				*m_listener;
	ChannelIDGenerator				*m_channelID_set;		//用户连接ID列表
	MessageQueueAB					*m_pMsgQueueAB;

	vector<Channel*>					m_Channels;
	vector<WorkerThread*>			m_libevent_threads;
	int m_last_thread;

	MessagePackage						m_msgPack;

	std::shared_ptr<std::thread> m_dispatchThread;// 消息派发线程


};

