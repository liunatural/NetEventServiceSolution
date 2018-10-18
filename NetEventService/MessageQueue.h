//**************************************************************************
//  File......... : MessageQueue.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : Head file of the class MessageQueue used as storing message packages 
//							concurrently for multi-threads 
//  History...... : first created by Liu Zhi 2018-10
//
//***************************************************************************
#pragma once
#include <mutex>
#include <deque>
#include "NetEventService.h"

class MessageQueue: public MsgQueue
{
public:
	MessageQueue();
	~MessageQueue();
	std::deque<MessagePackage> m_queue;

	//返回一个消息
	const MessagePackage*	GetMsg(unsigned int index);
	MessagePackage*	 GetLastMsg();
	int		GetCount();
	void	RemoveFront();
	void	Add(const MessagePackage& frame);
	void	Reset();

};


//多线程同步队列
class MessageQueueAB
{
public:
	MessageQueueAB();
	virtual ~MessageQueueAB();
	void	Push(const MessagePackage& msg);
	 MessageQueue&	Swap();
	 MessageQueue&	Get();

protected:
	MessageQueue	m_queue[2];
	int		m_current;
	int		m_queueMax;
	std::mutex counter_mutex;
};
