//****************************************************************************************
//  File......... : NetEventService.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : Head file of the class NetEventService API.
//
//  History...... : First created by Liu Zhi 2018-09
//
//****************************************************************************************

#pragma once

#ifdef NETEVENTSERVICE_EXPORTS
#define NETEVENTSERVICE_API __declspec(dllexport)
#else
#define NETEVENTSERVICE_API __declspec(dllimport)
#endif

#include "Message.h"

/*****************************************************************************************/
/////////////////////											��Ϣ���нӿڣ����ڴ洢���յ���Ϣ							//////////////////
/*****************************************************************************************/
class MsgQueue
{
public:
	MsgQueue() {}
	virtual ~MsgQueue() {}

	//����һ����Ϣ��
	virtual	const MessagePackage*	GetMsg(unsigned int index) = 0;
	
	//��ȡ�����е���Ϣ����
	virtual	int						GetCount() = 0;

};



/*****************************************************************************************/
/////////////////////												 ������Ϣ���񣺿ͻ��˽ӿ�									//////////////////
/*****************************************************************************************/

typedef void(__stdcall *fn_on_event_cb)(int& msgID);

class NetEvtClient
{
public:
	NetEvtClient() {};
	virtual ~NetEvtClient() {};

	//���ӷ�����
	virtual	int	 Connect(const char* ip, const char* port) = 0;
	
	//��ʼ��Ϣ�շ�����
	virtual	void Start() = 0;
	
	//��ȡ��Ϣ����
	virtual	MsgQueue&	GetMsgQueue() = 0;
	
	//�Ͽ�����
	virtual	void	Disconnect() = 0;

	//������Ϣ��
	virtual	int Send(MessagePackage& msg) = 0;
	
	//������Ϣ��id1:��ϢID, id2:����ID, data: ���͵�����, len:���ݳ��ȣ�
	virtual	int	 Send(unsigned short id1, unsigned short id2, const char* data, unsigned int len) = 0;

	virtual void SetEventCallback(fn_on_event_cb fncb) = 0;

};


/*****************************************************************************************/
/////////////////////												 ������Ϣ���� �������˽ӿ�								//////////////////
/*****************************************************************************************/
class NetEvtServer
{
public:
	NetEvtServer() {};
	virtual ~NetEvtServer() {};

	//������Ϣ�շ�����
	virtual	bool	Start(int port, int maxConnects = 1000) = 0;
	
	//ֹͣ��Ϣ�շ�����
	virtual	bool	Stop() = 0;

	//��ȡ��Ϣ����
	virtual	MsgQueue&	 GetMsgQueue() = 0;

	//������Ϣ��
	virtual	int		Send(int connectid, MessagePackage& msg) = 0;

	//������Ϣ��id1:��ϢID, id2:����ID, data: ���͵�����, len:���ݳ��ȣ�
	virtual	int		Send(int connectid, unsigned short id1, unsigned short id2, const void* data, unsigned int len) = 0;

	//�Ͽ���ĳ���û�������
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


/*****************************************************************************************/
/*----------------------------------------		      �����ļ���ȡ��			--------------------------------------------------*/		
/*****************************************************************************************/
class ConfigService
{
public:
	ConfigService() {}
	virtual ~ConfigService() {}

	//��XML�ļ������жϷ���ֵ
	virtual bool				OpenFile(const char* file) = 0;
	
	//����·����ȡXML�ڵ���ַ���ֵ
	virtual bool				GetStr(const char* path, char* rel) = 0;
	
	//����·����ȡXML�ڵ������ֵ
	virtual int					GetInt(const char* path) = 0;
	
	//����·����ȡXML�ڵ�ĸ�����ֵ
	virtual double				GetDouble(const char* path) = 0;
};

NETEVENTSERVICE_API ConfigService* CreateConfigReader();
