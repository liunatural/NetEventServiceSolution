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

	m_readBuffer = new DynamicBuffer();

}

Channel::~Channel()
{
	if (m_fd != -1)
	{
		evutil_closesocket(m_fd);
		//bufferevent_free(m_bev);
	}

	if (m_readBuffer)
	{
		delete m_readBuffer;
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
	m_readBuffer->Push(data, len);

	ReadPackage();

}

void Channel::ReadPackage()
{
	while (m_readBuffer->Size() >= MessagePackage::header_length)
	{
		int len = *(int*)m_readBuffer->Peek();
		if (m_readBuffer->Size() < len + MessagePackage::header_length)
		{
				break;
		}
	
		char* data = m_readBuffer->Peek();
		MessagePackage msgPack;
		memcpy(msgPack.data(), data, len + MessagePackage::header_length);
		msgPack.SetLinkID(m_channelID);//将数据包加上连接ID

		m_pMsgQAB->Push(msgPack);

		m_readBuffer->Pop(len + MessagePackage::header_length);
	}

}

void Channel::CloseSocket()
{
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

		//将用户断开连接信息返回给上层应用
		MessagePackage msgPack;
		msgPack.WriteHeader(link_disconnected, 0);
		msgPack.WriteBody((void*)m_ip.c_str(), dataLen);
		msgPack.SetLinkID(m_channelID);

		m_pMsgQAB->Push(msgPack);
	}
}


int Channel::SendData(void* data, int len)
{
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

