using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace CapsuleClient
{
    public class MessageBuffer
    {
        public void Push(byte[] pBuf, int len)
	    {
            byte[] tmp = new byte[len];
            System.Buffer.BlockCopy(pBuf, 0, tmp, 0, len);
            m_StreamBuffer.AddRange(tmp);
        }

        public void Pop(int nLen)
        {

            m_offerset += nLen;
            if (m_offerset > m_Capacities)
            {
                m_StreamBuffer.RemoveRange(0, m_offerset);
                m_offerset = 0;
             }
        }

        public byte[] Peek()
        {
            if (m_StreamBuffer.Count == 0)
            {
                return null;
            }

            byte[] data =  m_StreamBuffer.GetRange(m_offerset, Size()).ToArray();

            return data;
        }

        public int Size()
        {
            return (int)m_StreamBuffer.Count - m_offerset;
        }

        public void Clear()
        {
            m_StreamBuffer.Clear();
        }

        private const int m_Capacities = 1024 * 1024;
        private List<byte> m_StreamBuffer = new List<byte>();
        private int m_offerset = 0;

    }
}
