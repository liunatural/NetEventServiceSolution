using System;
using System.Text;
using System.Net;
using System.Net.Sockets;

namespace CapsuleClient
{
    public class VRHostControllerClient
    {
        //连接到VR主机控制服务器
        public void connectServer(string ip, string port)
        {
            m_client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            m_ip = IPAddress.Parse(ip);                                            //场景控制服务器IP
            m_point = new IPEndPoint(m_ip, int.Parse(port));            //端口

            try
            {
                m_client.Connect(m_point);
            }
            catch (Exception ex)
            {
                throw (ex);
            }
        }

        //请求座位号
        public bool RequestSeatNumber(string userID, ref int seatNumber)
        {
            bool bRet = false;

            //生成UserID数据包
            byte[] byte_userID = Encoding.UTF8.GetBytes(userID);

            CSMsgPackage msg = new CSMsgPackage();
            msg.WriteBody(byte_userID, byte_userID.Length);
            msg.WriteHeader(Protocol.ID_User_Login, Protocol.c2s_tell_user_id);

            //发送UserID数据包
            m_client.Send(msg.data(), msg.GetPackageLength(), SocketFlags.None);


            UserInfo userInfo = new UserInfo();
            GetSeatNumResponseMsg(ref userInfo);

            //for debug
            string strUID = Encoding.UTF8.GetString(userInfo.UserID);
            Console.WriteLine("UserID:" + strUID);
            Console.WriteLine("SeatNumber:" + userInfo.SeatNumber.ToString());
            if (strUID.CompareTo(userID) == 0 && userInfo.SeatNumber != -1)
            {
                seatNumber = userInfo.SeatNumber;
                bRet = true;
            }
            
            return bRet;

        }

        //处理VR主机控制器返回的座位号消息包
        public void GetSeatNumResponseMsg(ref UserInfo usrInfo)
        {
            int len = 0;

            byte[] rcBuf = new byte[8 * 1024];

            while (true)
            {
                try
                {
                    len = m_client.Receive(rcBuf);
                    m_buffer.Push(rcBuf, len);

                    while (m_buffer.Size() >= CSMsgPackage.header_length)
                    {
                        //取出消息头header
                        Header header = CSMsgPackage.Toheader(m_buffer.Peek());

                        int msgID = header.id1;
                        int cmdID = header.id2;

                        //处理主机控制器返回的座位号消息包
                        if (msgID == Protocol.ID_SceneCntrl_Notify && cmdID == Protocol.s2c_rsp_seat_num)
                        {
                            //弹出header
                            m_buffer.Pop(CSMsgPackage.header_length);
                          
                             usrInfo.FromBuf(m_buffer.Peek());

                            //弹出UserInfo
                            m_buffer.Pop(header.length);

                            return;
                        }
                        else
                        {
                            //其他类型消息包，暂不处理
                            m_buffer.Pop(CSMsgPackage.header_length + header.length);
                        }

                    }
                }
                catch (Exception ex)
                {
                    throw(ex);
                }
            }

        }

        public Socket m_client;
        public IPAddress m_ip;
        public IPEndPoint m_point;
        private MessageBuffer m_buffer = new MessageBuffer();

    }
}


