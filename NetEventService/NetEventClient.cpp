#include "NetEventClient.h"
#include "MessageQueue.h"
#include "protocol.h"

NetEventClient::NetEventClient() 
{
	m_pMsgQueueAB = new MessageQueueAB();
}


NetEventClient::~NetEventClient()
{
}

int NetEventClient::Connect(const char* ip, const char* port)
{

	WSADATA wsaData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		return -1;
	}


	m_pBase = event_base_new();
	if (!m_pBase)
	{
		printf("Couldn't open event base!\n");
		return -1;
	}


	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	int iPort = atoi(port);
	server_addr.sin_port = htons(iPort);
	server_addr.sin_addr.s_addr = inet_addr(ip);


	m_bev = bufferevent_socket_new(m_pBase, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!m_bev)
	{
		printf("Error constructing bufferevent!");
		return -1;
	}


	bufferevent_setcb(m_bev, readcb, NULL, eventcb, this);
	bufferevent_enable(m_bev, EV_READ | EV_WRITE | EV_PERSIST);

	if (bufferevent_socket_connect(m_bev, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("Error starting connection!\n");
		bufferevent_free(m_bev);
		return -1;
	}

	return 0;
}


void NetEventClient::Start()
{
	event_base_dispatch(m_pBase);
}

void NetEventClient::Disconnect()
{

	if (NULL != m_bev)
	{
		bufferevent_free(m_bev);
	}

	if (NULL != m_pBase)
	{
		event_base_free(m_pBase);
	}

}

MsgQueue& NetEventClient::GetMsgQueue()
{
	return m_pMsgQueueAB->Swap();
}

int NetEventClient::Send(MessagePackage& msg)
{
	//return bufferevent_write(m_bev, msg.data(), msg.GetPackageLength());

	evutil_socket_t fd = bufferevent_getfd(m_bev);
	int ret = send(fd, msg.data(), msg.GetPackageLength(), 0);

	return ret;
}

int NetEventClient::Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len)
{

	if (id1 < 0 || id2 < 0 || len < 0)
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

	return Send(m_msgPack);

}

void NetEventClient::readcb(struct bufferevent* bev, void* arg)
{

	NetEventClient* pNetEventClient = (NetEventClient*)arg;

	char data[1024 * 4] = { 0 };
	int nbytes = 0;
	int TotalBytes = 0;

	if (bev == NULL)
	{
		return;
	}

	int len = (int)bufferevent_read(bev, data, sizeof(data));
	pNetEventClient->GetDataStream().Push(data, len);

	pNetEventClient->read_pack();

}


void NetEventClient::read_pack()
{
	while (m_readStream.Size() >= 8)
	{

		int len = *(int*)m_readStream.Peek();
		if (m_readStream.Size() < len + 8)
			break;

		char* data = m_readStream.Peek();
		
		memcpy(m_msgPack.data(), data, len + 8);

		m_pMsgQueueAB->Push(m_msgPack);

		m_readStream.Pop(len + 8);
	}

}

void NetEventClient::eventcb(struct bufferevent *bev, short event, void *arg)
{

	if (event & BEV_EVENT_EOF)
	{
		printf("�������˶Ͽ����ӣ�\n");
	}
	else if (event & BEV_EVENT_ERROR)
	{
		printf("�������˷������󣡣���\n");
	}
	else if (event & BEV_EVENT_CONNECTED)
	{
		printf("�ɹ����ӷ�������\n");
		evutil_socket_t fd = bufferevent_getfd(bev);
		evutil_make_socket_nonblocking(fd);
	}

}