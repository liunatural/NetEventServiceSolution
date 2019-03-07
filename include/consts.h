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
#define	MAX_LOG_TEXT_LENGTH			8192					//最大LOG输出的一行文本为8192字节
#define MAX_PATH_LEN							250
#define MAX_SEND_MSGQ_LEN				8192
#define IP_ADDR_LENGTH						15
#define IP_PORT_LENGTH						6

enum ERROR_CODE
{
	SUCCESS									= 0,
	FAIL											= -1,
	
	NO_AVAILABLE_SEAT_NUMBER	= 1,					//没有可用的座席供分配
	PLAYER_ID_NOT_EXISTS			= 2,					//玩家ID在列表中不存在	
	INVALID_SEAT_NUMBER,									//无效的座席号

	//--------------------------中心服务器相关错误----------------------------//
	ERR_CONNECT_CENTER_SERVER = 2100,			//连接中心服务器失败

};

//客户端类型
enum UserType
{
	VIP					= 1,
	Capsule				= 2,
	VideoCamera,
	SceneServer,
	ExternalVIP,
	VRClientAgent,
};


//远程终端机的状态
enum DeviceState
{
	RecycleDisable	= 0,					//不可回收
	RecycleEnable	= 1,					//可回收
	Recycling			= 2,					//正在回收中
};
