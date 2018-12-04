﻿using System;

namespace CapsuleClient
{
    class Program
    {

        static void Main(string[] args)
        {
            //1. 创建场景控制器客户端
            SceneControllerClient sceneCtrlClient = new SceneControllerClient();

            try
            {
                //2. 连接场景控制器
                sceneCtrlClient.connectServer("127.0.0.1", "10001");
                ShowMsg("连接到场景控制服务器成功: " + sceneCtrlClient.m_client.RemoteEndPoint.ToString());


                //3. 请求座位号
                string userID = "useridxxxxxxxxx00001";
                int seatNumber = -1;
                bool bRet = sceneCtrlClient.RequestSeatNumber(userID, ref seatNumber);

                if (bRet)
                {
                    ShowMsg(seatNumber.ToString());

                }

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
