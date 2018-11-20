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
#define FILE_NAME_LENGTH					60
#define SCENE_SERVER_ID_LENGTH		32
#define MIN_SEAT_NUMBER					1
#define MAX_SEAT_NUMBER					30
#define MAX_DATA_BUFFER_LEN				(1024 * 1024)
#define	MAX_LOG_TEXT_LENGTH			8192					//���LOG�����һ���ı�Ϊ8192�ֽ�
#define MAX_PATH									250
#define MAX_SEND_MSGQ_LEN				8192

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
	OBSServer,
	ExternalVIP,
	VRClientAgent,
};

//�û�״̬
enum UserState
{
	state_initial = 0,
	state_entering,
	state_ready,
	state_left,
	state_present_flowers,					//����׻�
	state_stop_present_flowers,			//����׻�	
};

