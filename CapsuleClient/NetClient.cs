using System;
using System.Collections.Generic;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace CapsuleClient
{
    public class NetClient
    {
        public  void connectServer()
        {
            client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            ip = IPAddress.Parse("127.0.0.1");
            point = new IPEndPoint(ip, int.Parse("10001"));

            try
            {
                //连接到服务器
                client.Connect(point);
                ShowMsg("连接成功");
                ShowMsg("服务器" + client.RemoteEndPoint.ToString());
                ShowMsg("客户端:" + client.LocalEndPoint.ToString());


                string userID = "UserID0001";

                byte[] byte_userID = System.Text.Encoding.Default.GetBytes(userID);

                CSMsgPackage msg = new CSMsgPackage();
                msg.WriteBody(byte_userID, byte_userID.Length);
                msg.WriteHeader(Protocol.ID_User_Login, Protocol.c2s_tell_user_id);

                int ret = client.Send(msg.data(), msg.GetPackageLength(), SocketFlags.None);
                ReceiveMsg();


               ret = client.Send(msg.data(), msg.GetPackageLength(), SocketFlags.None);
               ReceiveMsg();

                ret = client.Send(msg.data(), msg.GetPackageLength(), SocketFlags.None);
                ReceiveMsg();


                //Thread th = new Thread(ReceiveMsg);
                //th.IsBackground = true;
                //th.Start();
            }
            catch (Exception ex)
            {
                ShowMsg(ex.Message);
            }
        }

        public void ReceiveMsg()
        {
            int len = 0;

            byte[] rcBuf = new byte[8 * 1024];

            while (true)
            {
                try
                {

                    len = client.Receive(rcBuf);
                    m_buffer.Push(rcBuf, len);

                    while (m_buffer.Size() >= CSMsgPackage.header_length)
                    {

                        Header _header = CSMsgPackage.Toheader(m_buffer.Peek());

                        int msgID = _header.id1;
                        int cmdID = _header.id2;

                        if (msgID == Protocol.ID_SceneCntrl_Notify && cmdID == Protocol.s2c_rsp_seat_num)
                        {
                            m_buffer.Pop(CSMsgPackage.header_length);

                            UserInfo _userInfo = new UserInfo();
                            _userInfo.FromBuf(m_buffer.Peek());

                            string str = System.Text.Encoding.Default.GetString(_userInfo.UserID);
                            ShowMsg("UserID:" + str);
                            ShowMsg("SeatNumber:" + _userInfo.SeatNumber.ToString());

                            m_buffer.Pop(_header.length);

                            return;
                        }
                        else
                        {
                            m_buffer.Pop(CSMsgPackage.header_length + _header.length);
                        }

                    }
                }
                catch (Exception ex)
                {
                    ShowMsg(ex.Message);
                    break;
                }
            }

        }

        public  void sendMsg()
        {
            //客户端给服务器发消息
            if (client != null)
            {
                try
                {
                    byte[] buffer = Encoding.UTF8.GetBytes("userid00001");
                    client.Send(buffer);
                }
                catch (Exception ex)
                {
                    ShowMsg(ex.Message);
                }
            }
        }

        void ShowMsg(string msg)
        {
           Console.WriteLine(msg);
        }

        Socket client;
        IPAddress ip;
        IPEndPoint point;

        MessageBuffer m_buffer = new MessageBuffer();

    }
}


