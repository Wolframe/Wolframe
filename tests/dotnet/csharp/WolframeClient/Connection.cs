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
        private IPAddress m_address;
        private int m_port;
        private TcpClient m_client;
        private NetworkStream m_stream;
        private Protocol.Buffer m_buffer;
        private bool m_readRequestIssued;
        private AutoResetEvent m_signal;
        private Queue<byte[]> m_writequeue;
        private bool m_waitwrite;
        private object m_waitwriteLock;

        public Connection(string ipadr, int port, AutoResetEvent signal)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_buffer = null;
            m_readRequestIssued = false;
            m_signal = signal;
            m_writequeue = new Queue<byte[]>();
            m_waitwrite = false;
            m_waitwriteLock = new object();
        }

        public void Connect()
        {
            m_client.Connect( m_address, m_port);
            m_stream = m_client.GetStream();
            m_buffer = new Protocol.Buffer( m_stream, 4096);
        }

        public void Close()
        {
            if (m_stream != null)
            {
                m_stream.Close();
                m_stream = null;
            }
            m_client.Close();
        }

        public bool HasReadData()
        {
            return m_buffer != null && m_buffer.HasData();
        }

        public byte[] ReadLine()
        {
            if (m_buffer == null) throw new Exception("read line failed");
            m_readRequestIssued = false;
            byte[] ln = m_buffer.FetchLine();
            /*[-]*/Console.WriteLine("+++ LINE {0}", Encoding.UTF8.GetString(ln));
            return ln;
        }

        public byte[] ReadContent()
        {
            if (m_buffer == null) throw new Exception("read content failed");
            m_readRequestIssued = false;
            byte[] msg = m_buffer.FetchContent();
            Console.WriteLine("+++ MSG");
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

        public void IssueReadRequest()
        {
            if (m_buffer == null) throw new Exception("issue read request failed");
            if (!m_readRequestIssued)
            {
                m_buffer.IssueRead(m_signal);
                m_readRequestIssued = true;
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
