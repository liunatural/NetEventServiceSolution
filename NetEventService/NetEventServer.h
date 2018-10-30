//**************************************************************************
//  File......... : NetEventServer.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : head file of the class NetEventServer used as encapsulation to common 
//							functions in libevent open source library for network communication.
//  History...... : first created by Liu Zhi 2018-09
//
//***************************************************************************
#pragma once
#include "NetEventService.h"
#include "MessageQueue.h"
#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include <vector>
#include <queue>
#include <mutex>
#include <iostream>
#include "CommonMsgQueue.h"


using namespace std;

class NetEventServer;
class Channel;
class Channel2;
class ChannelManager;
class MessageQueueAB;
enum send_stat;




typedef struct 
{
	evutil_socket_t fd;
	int tid;						//threadID
	std::string ip;
}conn_queue_item;

//�����߳̽ṹ��
typedef struct 
{
	NetEventServer* that;									//��������
	std::shared_ptr<std::thread>   spThread;		// �߳�
	struct event_base * thread_base;					// �¼���
	struct event   notify_event;
	evutil_socket_t  notfiy_recv_fd;						// socketpair ���ն�fd�������߳̽���֪ͨ��
	evutil_socket_t  notfiy_send_fd;						// socketpair ���Ͷ�fd�������̷߳���֪ͨ��
	std::mutex conn_mtx;									 //ά�����Ӷ��е���
	std::queue<conn_queue_item>  conn_queue;  //����conn_queue_item�Ķ���

	bool m_bStop;
}ReceiverThread;



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
	MessageQueueAB* GetMessageQueueAB() { return m_pMsgQueueAB; }
	std::vector<Channel*>* GetChannels() { return &m_Channels; }
	ChannelManager* GetChannelManager() { return m_channelMgr; }
	CommonMsgQueue<MessagePackage>* GetSendMsgQueue() { return m_pSendMsgQueue; }
	bool GetStopSenderThreadFlag() { return m_bStopThread; }
	int DoSend(MessagePackage& msg);
	int GetOnlineAmount();


private:
	static void notify_cb(evutil_socket_t fd, short which, void *args);
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *user_data);
	static void conn_readcb(struct bufferevent *bev, void *arg);
	static void conn_eventcb(struct bufferevent *bev, short what, void *arg);
	static void sender_thread_task(NetEventServer *pNetEvtSvr);

	bool Init(int start, int size);
	bool StartReceiverThreads(int thread_numb);
	void StartSenderThreads(int thread_numb);
	bool SetupReceiverThread(ReceiverThread * pLibeventThread);
	Channel* CreateChannel(bufferevent *bev, conn_queue_item& connItem);

private:
	std::mutex							m_channel_mtx;
	struct event_base					*m_base;
	struct evconnlistener				*m_listener;
	ChannelManager						*m_channelMgr;		//�û�����ID������
	MessageQueueAB					*m_pMsgQueueAB;
	CommonMsgQueue<MessagePackage>  *m_pSendMsgQueue;
	vector<Channel*>					m_Channels;
	vector<ReceiverThread*>		m_libevent_threads;
	vector<std::thread>				m_send_threads;
	int m_last_thread;
	int											m_thread_num;
	MessagePackage						m_msgPack;

	std::shared_ptr<std::thread> m_dispatchThread;// ��Ϣ�ɷ��߳�


	volatile bool m_bStopThread;

};

