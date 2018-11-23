using System;
using System.Collections.Generic;
using System.Text;



namespace CapsuleClient
{

 
    class Program
    {

        static void Main(string[] args)
        {

            NetClient cli = new NetClient();

            cli.connectServer();
        }

    }

}
