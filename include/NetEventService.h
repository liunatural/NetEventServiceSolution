#pragma once

#ifdef NETEVENTSERVICE_EXPORTS
#define NETEVENTSERVICE_API __declspec(dllexport)
#else
#define NETEVENTSERVICE_API __declspec(dllimport)
#endif

#include "Message.h"

class MsgQueue
{
public:
	MsgQueue() {}
	virtual ~MsgQueue() {}
	virtual	const MessagePackage*	GetMsg(unsigned int index) = 0;
	virtual	int						GetCount() = 0;
};

class NetEvtClient
{
public:
	NetEvtClient() {};
	virtual ~NetEvtClient() {};
	virtual	int	 Connect(const char* ip, const char* port) = 0;
	virtual	void Start() = 0;
	virtual	void	Disconnect() = 0;
	virtual	MsgQueue&	GetMsgQueue() = 0;
	virtual	int Send(MessagePackage& msg) = 0;
	virtual	int	 Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len) = 0;
};


class NetEvtServer
{
public:
	NetEvtServer() {};
	virtual ~NetEvtServer() {};
	virtual	bool	Start(int port, int maxConnects = 1000) = 0;
	virtual	bool	Stop() = 0;
	virtual	MsgQueue&	 GetMsgQueue() = 0;
	virtual	int		Send(int connectid, MessagePackage& msg) = 0;
	virtual	int		Send(int connectid, unsigned short id1, unsigned short id2, const void* data, unsigned int len) = 0;
	virtual	void	Close(int connectid) = 0;
};


NETEVENTSERVICE_API NetEvtServer*  CreateNetEvtServer();

NETEVENTSERVICE_API NetEvtClient*  CreateNetEvtClient();