//**************************************************************************
//  File......... : MessageQueue.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : implementaion file of the class MessageQueue used to 
//
//  History...... : first created by Liu Zhi 2018-09
//
//**************************************************************************


#include "MessageQueue.h"


MessageQueue::MessageQueue()
{
}


MessageQueue::~MessageQueue()
{
}


const MessagePackage* MessageQueue::GetMsg(unsigned int index)
{
	if (index >= m_queue.size())
		return 0;
	return &m_queue[index];
}

MessagePackage*	MessageQueue::GetLastMsg()
{
	if ( m_queue.size() == 0)
		return 0;
	return &m_queue.back();
}


//返回消息包数量
int		MessageQueue::GetCount() 
{
	return (int)m_queue.size();
}


void	MessageQueue::RemoveFront()
{
	m_queue.pop_front();
}


void	MessageQueue::Add(const MessagePackage& frame)
{
	m_queue.push_back(frame);
}


void	MessageQueue::Reset()
{
	m_queue.clear();
}


/////////////////////////////////////////////////////////////////////////////////
MessageQueueAB::MessageQueueAB()
{
	m_current = 0;
	m_queueMax = 4096;
}

MessageQueueAB::~MessageQueueAB()
{

}


void MessageQueueAB::Push(const MessagePackage& msg)
{
	std::unique_lock<std::mutex> lck(counter_mutex);

	m_queue[m_current].Add(msg);

	if (m_queue[m_current].GetCount() >= m_queueMax)
		m_queue[m_current].RemoveFront();
}


MessageQueue& MessageQueueAB::Swap()
{
	std::unique_lock<std::mutex> lck(counter_mutex);

	int old = m_current;
	m_current = (m_current + 1) % 2;
	m_queue[m_current].Reset();
	return m_queue[old];
}


MessageQueue& MessageQueueAB::Get()
{
	return m_current == 0 ? m_queue[1] : m_queue[0];
}



