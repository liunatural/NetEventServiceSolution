#include "Channel.h"
#include "event2/buffer.h"
#include "event2/event.h"
#include "MessageQueue.h"
#include "Message.h"
#include "NetEventServer.h"
#include "ChannelIDGenerator.h"


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


void Channel::handle_read()
{
	char data[1024 * 4] = {0};
	int nbytes = 0;
	int TotalBytes = 0;

	if (m_bev == NULL)
	{
		return;
	}

	int len = (int)bufferevent_read(m_bev, data, sizeof(data));
	m_readStream.Push(data, len);

	read_pack();

}


void Channel::read_pack()
{

	while (m_readStream.Size() >= 8)
	{

		int len = *(int*)m_readStream.Peek();
		if (m_readStream.Size() < len + 8 )
			break;

		char* data = m_readStream.Peek();
		//short tid = (short)GetTID();
		//memcpy(data + 4, &tid, 2);  //加上线程号

		MessagePackage msgPack;
		memcpy(msgPack.data(), data, len + 8);

		msgPack.SetLinkID(m_channelID);

		m_pMsgQAB->Push(msgPack);

		m_readStream.Pop(len + 8);
	}

}


void Channel::close()
{
	std::lock_guard<std::mutex> lock(channel_mtx);

	if (m_fd != -1)
	{
		bufferevent_setfd(m_bev, -1);
		evutil_closesocket(m_fd);

		bufferevent_free(m_bev);

		m_fd = -1;

		m_bev = NULL;
		m_bUsedFlag = false;


		int  cid = GetChannelID();
		m_pNetEvtSvr->GetChannelIDSet()->freeId(cid); //归还ChannelID

		MessagePackage msgPack;
		msgPack.header()->id1 = link_stat::link_disconnected;
		msgPack.header()->id2 = 0;

		msgPack.SetLinkID(m_channelID);
		msgPack.SetBodyLength(0);

		m_pMsgQAB->Push(msgPack);
	}
}


//
//send_stat Channel::send_data(unsigned short id1, unsigned short id2, const void* data, int len)
//{
//	std::lock_guard<std::mutex> lock(channel_mtx);
//
//	int datalength = len;
//	MessagePackage msgPack;
//
//	msgPack.header()->id1 = id1;
//	msgPack.header()->id2 = id2;
//
//	if (data == NULL)
//	{
//		datalength = 0;
//	}
//	
//	if (datalength > 0)
//	{
//		memcpy(msgPack.body(), data, datalength);
//	}
//	
//	msgPack.SetBodyLength(datalength);
//
//	int ret = send(m_fd, msgPack.data(), datalength, 0);
//
//	return send_stat::send_succeed;
//}



send_stat Channel::send_data(void* data, int len)
{
	std::lock_guard<std::mutex> lock(channel_mtx);

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

	return send_stat::send_succeed;
}


//
//bool Channel::SetupConnection(evutil_socket_t fd)
//{
//	//m_fd = fd;
//	//evutil_make_socket_nonblocking(fd);
//	//bufferevent_setcb(m_bev, DoRead, NULL, DoError, this);
//	//bufferevent_setfd(m_bev, fd);
//	//if (bufferevent_enable(m_bev, EV_READ | EV_WRITE) != -1)
//	//{
//	//	m_bUsed = true;
//	//}
//	return m_bUsed;
//}
//
//
//
//void Channel::CloseConnection()
//{
//	//因为CloseConnection是在libevent线程的Event回掉中调用的，直接
//	//bufferevent_setfd就会自动把之前的event给delete掉
//	//bufferevent_disable(m_be, EV_READ | EV_WRITE);
//	bufferevent_setfd(m_bev, -1);
//	evutil_closesocket(m_fd);
//	m_fd = -1;
//	//当bufferevent_setfd设置fd是-1的时候，并不会将write和read的event
//	//加入到event_base,如果event_base中监听的event数量为0时候，
//	//event_base_loop会退出，导致libevent线程结束，所以这里重新手动
//	//开启write,read事件
//	bufferevent_enable(m_bev, EV_READ | EV_WRITE);
//	m_bUsed = false;
//}