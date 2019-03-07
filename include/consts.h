//**************************************************************************
//  File......... : consts.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-09 
//  Description.. : common const definition file.
//
//  History...... : First created by Liu Zhi 2018-10
//
//***************************************************************************
#pragma  once

#define USER_ID_LENGTH						32
#define SERVER_ID_LENGTH					32
#define MAX_DATA_BUFFER_LEN				(1024 * 1024)
#define	MAX_LOG_TEXT_LENGTH			8192					//���LOG�����һ���ı�Ϊ8192�ֽ�
#define MAX_PATH_LEN							250
#define MAX_SEND_MSGQ_LEN				8192
#define IP_ADDR_LENGTH						15
#define IP_PORT_LENGTH						6

enum ERROR_CODE
{
	SUCCESS									= 0,
	FAIL											= -1,
	
	NO_AVAILABLE_SEAT_NUMBER	= 1,					//û�п��õ���ϯ������
	PLAYER_ID_NOT_EXISTS			= 2,					//���ID���б��в�����	
	INVALID_SEAT_NUMBER,									//��Ч����ϯ��

	//--------------------------���ķ�������ش���----------------------------//
	ERR_CONNECT_CENTER_SERVER = 2100,			//�������ķ�����ʧ��

};

//�ͻ�������
enum UserType
{
	VIP					= 1,
	Capsule				= 2,
	VideoCamera,
	SceneServer,
	ExternalVIP,
	VRClientAgent,
};


//Զ���ն˻���״̬
enum DeviceState
{
	RecycleDisable	= 0,					//���ɻ���
	RecycleEnable	= 1,					//�ɻ���
	Recycling			= 2,					//���ڻ�����
};
