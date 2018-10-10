//**************************************************************************
//
//  File......... : MessageQueue.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : head file of the  MessageQueue.
//
//  History...... : first created Han Liu Zhi 2018-09
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

	//����һ����Ϣ
	const MessagePackage*	GetMsg(unsigned int index);
	MessagePackage*	 GetLastMsg();
	int		GetCount();
	void	RemoveFront();
	void	Add(const MessagePackage& frame);
	void	Reset();

};


//���߳�ͬ������
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
