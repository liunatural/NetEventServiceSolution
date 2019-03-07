
namespace CapsuleClient
{
    class Protocol
    {
        //消息ID （业务层消息ID，从1000以后开始）
        public const ushort ID_User_Login = 1001;			        //用户登录消息ID
        public const ushort ID_HostCtlr_Notify = 1013;

        //命令ID
        public const ushort c2s_tell_user_id = 14;
        public const ushort s2c_rsp_seat_num = 104;				//下发座位号给胶囊体
    }
}
