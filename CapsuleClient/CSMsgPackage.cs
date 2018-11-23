using System;
using System.Collections.Generic;
using System.Text;



namespace CapsuleClient
{

     public struct Header
    {
        public Int32 length;
        public UInt16 id1;
        public UInt16 id2;
    };

    //存储用户基本信息
    public class UserInfo
    {
        public void FromBuf(byte[] buf)
        {
            System.Buffer.BlockCopy(buf, 0, UserID, 0, 33);
            SeatNumber = BitConverter.ToInt32(buf, 33);
        }

        public byte[] UserID = new byte[33];
        public int SeatNumber;
    };


    public class CSMsgPackage
    {
        public const int header_length = 8;
        public const int max_body_length = 8 * 1024;
        private byte[] m_buffer = new byte[header_length + max_body_length];

        public Header header()
        {
            Header _header = new Header();

            _header.length = BitConverter.ToInt32(m_buffer, 0);
            _header.id1 = BitConverter.ToUInt16(m_buffer, 4);
            _header.id2 = BitConverter.ToUInt16(m_buffer, 6);

            return _header;
        }

        public static Header Toheader(byte[] buffer)
        {
            Header _header = new Header();

            _header.length = BitConverter.ToInt32(buffer, 0);
            _header.id1 = BitConverter.ToUInt16(buffer, 4);
            _header.id2 = BitConverter.ToUInt16(buffer, 6);

            return _header;
        }


        public byte[] body()
        {
            byte[] _body = new byte[header().length];
            System.Buffer.BlockCopy(m_buffer, header_length, _body, 0, header().length);

            return _body;
        }

        public byte[] data()
        {
            return m_buffer;
        }

        public int GetBodyLength()
        {
            return header().length;
        }

        public int GetPackageLength()
        {
            return header_length + GetBodyLength();
        }

        public bool WriteBody(byte[] input, int len)
        {
            if (len < 0 || len > max_body_length)
            {
                return false;
            }

            input.CopyTo(m_buffer, header_length);
            BitConverter.GetBytes(len).CopyTo(m_buffer, 0);

            return true;
        }

        public bool WriteHeader(ushort msgID, ushort cmdID)
        {
            BitConverter.GetBytes(msgID).CopyTo(m_buffer, 4);
            BitConverter.GetBytes(cmdID).CopyTo(m_buffer, 6);
            return true;
        }

    }
  
}
