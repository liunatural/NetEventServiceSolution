//**************************************************************************
//  File......... : prototol.h
//  Project...... : VR                            
//  Author....... : Liu Zhi                                                 
//  Date......... : 2018-10 
//  Description.. : Protocol file used as communication between the business layer and core network layer.
//
//  History...... : First created by Liu Zhi 2018-10
//
//***************************************************************************

#pragma  once
#include "consts.h"

//系统消息ID（网络层消息ID）
enum 
{
	link_unconnect							= 1,
	link_connecting,
	link_connect_failed,
	link_disconnected,
	link_connected,
	link_error_del_channel,
	link_error_channel_is_exist,							//channel已经存在
	link_error_exceed_max_connects,					//超过最大连接数	

	send_succeed								= 20,
	send_buffer_full ,
	send_disconnected,
	send_parameter_error,
	send_target_not_exist,

	link_server_closed,
	link_server_failed,

};


//用户消息ID （业务层消息ID，从1000以后开始）
enum
{
	ID_User_Login							=1001,			//*********************用户登录消息ID
	ID_User_Notify,												//用户通知消息ID
	ID_User_Transform,										//用户数据变换消息ID
	ID_User_control,											//用户控制消息
	ID_Global_Notify,											//跨场景服务器的通知消息
	ID_Global_Transform,										//跨场景服务器的运动变换消息
	ID_HostCtlr_Notify						= 1013,			//**********************VR主机控制器通知消息
	ID_VRClientAgent_Notify,								//VR终端代理通知消息

	ID_ERROR,													//运行错误消息ID
};

//用户命令ID（业务层命令ID）
enum
{
	c2s_tell_seat_num						= 1,				//VIP客户端连接后发送座位号给场景服务器				****即将废除****
	c2s_tell_user_type,										//客户端类型：胶囊体客户端；VIP客户端；摄像机视角客户端	
	c2s_req_seat_num,										//胶囊客户端请求座位号
	c2s_seen_external,											//
	c2s_stop_seen_external,									//
	c2s_present_flowers,										//玩家发起献花	
	c2s_stop_present_flowers,								//玩家结束献花	


	c2s_tell_user_id							=14,				//*********************胶囊体告诉用户ID给VR主机控制器
	c2s_device_status_changed,							//设备状态 
	c2s_tell_user_info,



	s2c_ply_leave							= 102,			//服务器返回用户离开命令ID
	s2c_begin_flying,											//用户起飞命令
	s2c_rsp_seat_num						= 104,			//*********************VR主机控制器返回座位号给胶囊体
	s2c_upd_user_state,
	s2c_client_list,
	s2c_client_list_external,									//其它场景服务器的用户列表
	s2c_user_leave_external,								//外部用户离开
	s2c_trans_ext_usr_profile,								//传递外部用户描述数据
	
	s2c_tell_user_id,											//VR主机控制器告知用户ID
	s2c_device_status_changed,							//机器状态： free, used
	s2c_new_user_online,										//新用户上线


	s2s_req_usr_list							= 200,
	//最大协议数值254，大了会出错
};


#pragma pack(push, 1)

struct vec3
{
	float x;
	float y;
	float z;
};

struct TransformInfo
{
	int plyId;
	int seatNumber;				//座椅号
	vec3 pos;						//位置坐标
	vec3 head;						//头部朝向
	vec3 lhand;					//左手数据
	vec3 rhand;					//右手数据
	vec3 dir;						//身体朝向
	bool update;					//是否更新位置标志
	
	TransformInfo()
	{
		update = false;
	}

	TransformInfo(const TransformInfo& other)
	{
		this->pos = other.pos;
		this->head = other.head;
		this->dir = other.dir;
		this->lhand = other.lhand;
		this->rhand = other.rhand;
		this->plyId = other.plyId;
		this->seatNumber = other.seatNumber;
	}

	TransformInfo& operator = (const TransformInfo& other)
	{
		this->pos = other.pos;
		this->head = other.head;
		this->dir = other.dir;
		this->lhand = other.lhand;
		this->rhand = other.rhand;
		this->plyId = other.plyId;
		this->seatNumber = other.seatNumber;

		return *this;
	}
};

struct LeaveUserInfo
{
	char SceneServerID[SERVER_ID_LENGTH + 1] = { 0 };
	int mSeatNumber;
};


//存储用户基本信息
struct  UserInfo
{
	char	UserID[USER_ID_LENGTH + 1] = { 0 };
	int		SeatNumber = -1;
};


struct DeviceStatus
{
	int seatNumber;
	int status;
};


#pragma pack(pop)
