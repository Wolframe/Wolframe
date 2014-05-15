using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Threading;
using System.Collections.Generic;
using WolframeClient;

namespace WolframeClient
{
    class Connection
    {
        public delegate void DataReadyDelegate();

        private IPAddress m_address;
        private int m_port;
        private TcpClient m_client;
        private NetworkStream m_stream;
        private Protocol.Buffer m_buffer;
        private Queue<byte[]> m_writequeue;
        private bool m_waitwrite;
        private object m_waitwriteLock;
        private bool m_waitread;
        private object m_waitreadLock;
        private bool m_eofread;

        public Connection(string ipadr, int port)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_buffer = null;
            m_writequeue = new Queue<byte[]>();
            m_waitwrite = false;
            m_waitwriteLock = new object();
            m_waitread = false;
            m_waitreadLock = new object();
            m_eofread = false;
        }

        public void Connect()
        {
            m_client.Connect( m_address, m_port);
            m_stream = m_client.GetStream();
            m_buffer = new Protocol.Buffer(m_stream, 4096, IssueReadRequest);
            IssueReadRequest();
        }

        public void Close()
        {
            m_buffer.Close();
            m_stream.Close();
            m_client.Close();
        }

        public bool HasReadData()
        {
            return (m_buffer.HasData() || m_client.Available > 0);
        }

        public byte[] ReadLine()
        {
            byte[] ln = m_buffer.FetchLine();
            return ln;
        }

        public byte[] ReadContent()
        {
            byte[] msg = m_buffer.FetchContent();
            return msg;
        }

        public void EndWriteCallback(IAsyncResult ev)
        {
            m_stream.EndWrite(ev);
            lock (m_waitwriteLock)
            {
                m_waitwrite = false;
            }
            FlushWriteQueue();
        }

        public void FlushWriteQueue()
        {
            lock (m_waitwriteLock)
            {
                if (!m_waitwrite)
                {
                    try
                    {
                        byte[] msg = m_writequeue.Dequeue();
                        m_stream.BeginWrite(msg, 0, msg.Length, EndWriteCallback, null);
                        m_waitwrite = true;
                    }
                    catch (InvalidOperationException)
                    {}
                }
            }
        }

        public void EndReadCallback(IAsyncResult ev)
        {
            int nof_bytes_read;
            if (m_stream.CanRead)
            {
                nof_bytes_read = m_stream.EndRead(ev);
            }
            else
            {
                nof_bytes_read = 0;
            }
            if (nof_bytes_read == 0)
            {
                m_eofread = true;
                m_buffer.PushReadChunk(new Protocol.ReadChunk { ar = null, size = 0 } /*EOF*/);
            }
            else
            {
                m_buffer.PushReadChunk(new Protocol.ReadChunk { ar = ev.AsyncState as byte[], size = nof_bytes_read });
            }
            if (nof_bytes_read != 0)
            {
                m_waitread = false;
                IssueReadRequest();
            }
        }

        public void IssueReadRequest()
        {
            int readsize = 2048;
            if (m_client.Available > readsize)
            {
                readsize = m_client.Available;
            }
            bool doRead = false;
            lock (m_waitreadLock)
            {
                if (!m_waitread && !m_eofread)
                {
                    doRead = true;
                    m_waitread = true;
                }
            }
            if (doRead)
            {
                byte[] buf = new byte[readsize];
                m_stream.BeginRead(buf, 0, buf.Length, EndReadCallback, buf);
            }
        }

        public void WriteLine(string ln)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);

            Byte[] msg = utf8.GetBytes(ln);
            byte[] msg_with_EoLn = new byte[msg.Length + 2];
            Array.Copy( msg, msg_with_EoLn, msg.Length);
            msg_with_EoLn[msg.Length + 0] = (byte)'\r';
            msg_with_EoLn[msg.Length + 1] = (byte)'\n';
            m_writequeue.Enqueue(msg_with_EoLn);
            FlushWriteQueue();
        }

        public void WriteRequest(string command, byte[] content)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);
            byte[] hdr;
            if (command == null)
            {
                hdr = utf8.GetBytes("REQUEST\r\n");
            }
            else
            {
                hdr = utf8.GetBytes("REQUEST " + command + "\r\n");
            }
            byte[] content_encoded = Protocol.EscapeLFdot( content);
            int msglen = hdr.Length + content_encoded.Length + 5/*EoD marker*/;
            byte[] msg = new byte[msglen];
            Array.Copy(hdr, 0, msg, 0, hdr.Length);
            Array.Copy(content_encoded, 0, msg, hdr.Length, content_encoded.Length);
            int EoDidx = content_encoded.Length + hdr.Length;
            msg[EoDidx + 0] = (byte)'\r';
            msg[EoDidx + 1] = (byte)'\n';
            msg[EoDidx + 2] = (byte)'.';
            msg[EoDidx + 3] = (byte)'\r';
            msg[EoDidx + 4] = (byte)'\n';
            m_writequeue.Enqueue(msg);
            FlushWriteQueue();
        }
    };
}
