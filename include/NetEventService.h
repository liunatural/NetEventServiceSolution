//****************************************************************************************
//
//  File......... : NetEventService.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Head file of the class NetEventService used to declare service interface.
//
//  History...... : First created Liu Zhi 2018-09
//
//****************************************************************************************

#pragma once

#ifdef NETEVENTSERVICE_EXPORTS
#define NETEVENTSERVICE_API __declspec(dllexport)
#else
#define NETEVENTSERVICE_API __declspec(dllimport)
#endif

//���LOG�����һ���ı�Ϊ8192�ֽ�
#define	MAX_LOG_TEXT_LENGTH		8192


#include "Message.h"

/*****************************************************************************************/
/////////////////////											��Ϣ���нӿڣ����ڴ洢���յ���Ϣ							//////////////////
/*****************************************************************************************/
class MsgQueue
{
public:
	MsgQueue() {}
	virtual ~MsgQueue() {}
	virtual	const MessagePackage*	GetMsg(unsigned int index) = 0;
	virtual	int						GetCount() = 0;
};


/*****************************************************************************************/
/////////////////////												 ������Ϣ���񣺿ͻ��˽ӿ�									//////////////////
/*****************************************************************************************/
class NetEvtClient
{
public:
	NetEvtClient() {};
	virtual ~NetEvtClient() {};
	//��ʹ��֮ǰ�����ȵ��ô˺���
	virtual	int	 Connect(const char* ip, const char* port) = 0;
	virtual	void Start() = 0;
	virtual	void	Disconnect() = 0;
	virtual	MsgQueue&	GetMsgQueue() = 0;
	virtual	int Send(MessagePackage& msg) = 0;
	virtual	int	 Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len) = 0;
};


/*****************************************************************************************/
/////////////////////												 ������Ϣ���� �������˽ӿ�								//////////////////
/*****************************************************************************************/
class NetEvtServer
{
public:
	NetEvtServer() {};
	virtual ~NetEvtServer() {};

	//��ʹ��֮ǰ�����ȵ��ô˺���
	virtual	bool	Start(int port, int maxConnects = 1000) = 0;
	virtual	bool	Stop() = 0;
	virtual	MsgQueue&	 GetMsgQueue() = 0;
	virtual	int		Send(int connectid, MessagePackage& msg) = 0;
	virtual	int		Send(int connectid, unsigned short id1, unsigned short id2, const void* data, unsigned int len) = 0;
	virtual	void	Close(int connectid) = 0;
};

NETEVENTSERVICE_API NetEvtServer*		CreateNetEvtServer();
NETEVENTSERVICE_API NetEvtClient*			CreateNetEvtClient(); 



/*****************************************************************************************/
/////////////////////												 ��־API															//////////////////
/*****************************************************************************************/
enum LOG_TYPE
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

NETEVENTSERVICE_API void			InitLogger(const char* dir);//��ʹ��Log����֮ǰ�����ȵ��ô˺���  
NETEVENTSERVICE_API void			LOG(LOG_TYPE logType, const char* format, ...);

