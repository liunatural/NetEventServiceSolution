using System;

namespace CapsuleClient
{
    class Program
    {

        static void Main(string[] args)
        {
            //1. 创建VR主机控制器客户端
            VRHostControllerClient hostCtrlClient = new VRHostControllerClient();

            try
            {
                //2. 连接VR主机控制器
                hostCtrlClient.connectServer("192.168.2.86", "10002");
                ShowMsg("连接到VR主机控制服务器成功: " + hostCtrlClient.m_client.RemoteEndPoint.ToString());


                //3. 请求座位号
                string userID = "useridxxxxxxxxx00001";
                int seatNumber = -1;
                bool bRet = hostCtrlClient.RequestSeatNumber(userID, ref seatNumber);

                if (bRet)
                {
                    ShowMsg(seatNumber.ToString());
                }
                else
                {
                    ShowMsg("VR主机控制器返回无效的座位号！");
                }

                Console.ReadLine();

            }
            catch (Exception ex)
            {
                ShowMsg(ex.Message);
            }
        }
        static void ShowMsg(string msg)
        {
            Console.WriteLine(msg);
        }

    }
}
