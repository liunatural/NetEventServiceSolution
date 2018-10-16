//**************************************************************************
//  File......... : Channel.cpp
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Implementation file for class Channel used as user messages processing 
//						and user connection data managements.
//
//  History...... : First created by Liu Zhi 2018-09
//**************************************************************************

#include "Channel.h"
#include "event2/buffer.h"
#include "event2/event.h"
#include "MessageQueue.h"
#include "NetEventServer.h"
#include "ChannelManager.h"
#include "protocol.h"


Channel::Channel(struct bufferevent *bev) 
{
	m_bev = bev;
	m_bUsedFlag = false;
}

Channel::~Channel()
{
	if (m_fd != -1)
	{
		evutil_closesocket(m_fd);
		bufferevent_free(m_bev);
	}
}


MessageQueueAB* Channel::GetMsgQueueAB() 
{ 
	return m_pMsgQAB; 
}


void   Channel::SeMsgQueueAB(MessageQueueAB* pMsgQAB)
{
	m_pMsgQAB = pMsgQAB;
}


NetEventServer* Channel::GetNetEventServer()
{
	return m_pNetEvtSvr;
}

void Channel::DoRead()
{
	char data[1024 * 4] = {0};
	int nbytes = 0;
	int TotalBytes = 0;

	if (m_bev == NULL)
	{
		return;
	}

	int len = (int)bufferevent_read(m_bev, data, sizeof(data));
	m_readBuffer.Push(data, len);

	ReadPackage();

}

void Channel::ReadPackage()
{

	while (m_readBuffer.Size() >= 8)
	{

		int len = *(int*)m_readBuffer.Peek();
		if (m_readBuffer.Size() < len + 8 )
			break;

		char* data = m_readBuffer.Peek();
		
		//short tid = (short)GetTID();
		//memcpy(data + 4, &tid, 2);  //加上线程号

		MessagePackage msgPack;
		memcpy(msgPack.data(), data, len + 8);

		msgPack.SetLinkID(m_channelID);

		m_pMsgQAB->Push(msgPack);

		m_readBuffer.Pop(len + 8);
	}

}

void Channel::CloseChannel()
{
	std::lock_guard<std::mutex> lock(m_channel_mtx);

	if (m_fd != -1)
	{
		bufferevent_setfd(m_bev, -1);
		evutil_closesocket(m_fd);

		bufferevent_free(m_bev);

		m_fd = -1;

		m_bev = NULL;
		m_bUsedFlag = false;


		int  cid = GetChannelID();
		m_pNetEvtSvr->GetChannelManager()->ReleaseID(cid); //归还ChannelID

		int dataLen = strlen(m_ip.c_str());

		MessagePackage msgPack;
		msgPack.WriteHeader(link_disconnected, 0);
		msgPack.WriteBody((void*)m_ip.c_str(), dataLen);
		msgPack.SetLinkID(m_channelID);


		m_pMsgQAB->Push(msgPack);
	}
}


int Channel::SendData(void* data, int len)
{
	std::lock_guard<std::mutex> lock(m_channel_mtx);

	if (m_fd == -1)
	{
		return send_disconnected;
	}

	//当用户断开连接时， bufferevent_write函数会死锁在m_bev里不返回
	//if (0 != bufferevent_write(m_bev, data, len))
	//{
	//	printf("bufferevent_write  error\n");
	//	return send_stat::send_buffer_full;
	//}

	int ret = send(m_fd, (const char*)data, len, 0); 

	return send_succeed;
}

