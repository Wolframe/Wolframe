using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Threading;
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
        private object m_readRequestIssuedLock;
        private AutoResetEvent m_signal;

        public Connection(string ipadr, int port, AutoResetEvent signal)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_buffer = new Protocol.Buffer( 4096);
            m_readRequestIssued = false;
            m_readRequestIssuedLock = new object();
            m_signal = signal;
        }

        public void Connect()
        {
            m_client.Connect( m_address, m_port);
            m_stream = m_client.GetStream();
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
            return m_buffer.HasData();
        }

        public byte[] ReadLine()
        {
            return m_buffer.FetchLine(m_stream);
        }

        public byte[] ReadContent()
        {
            return m_buffer.FetchContent(m_stream);
        }

        public void ReadCallback(IAsyncResult result)
        {
            int readlen = m_stream.EndRead(result);
            if (readlen <= 0)
            {
                Close();
            }
            else
            {
                if (m_buffer.pos + readlen <= m_buffer.size)
                {
                    m_buffer.pos = m_buffer.pos + readlen;
                }
                lock (m_readRequestIssuedLock)
                {
                    m_readRequestIssued = false;
                }
                m_signal.Set();
            }
        }

        public void IssueReadRequest()
        {
            lock (m_readRequestIssuedLock)
            {
                if (!m_readRequestIssued)
                {
                    m_stream.BeginRead(m_buffer.ar, m_buffer.pos, m_buffer.size - m_buffer.pos, ReadCallback, null);
                    m_readRequestIssued = true;
                }
            }
        }

        public void WriteLine(string ln)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);

            Byte[] msg = utf8.GetBytes(ln);
            byte[] msg_with_EoLn = new byte[msg.Length + 2];
            msg_with_EoLn[msg.Length + 0] = (byte)'\r';
            msg_with_EoLn[msg.Length + 1] = (byte)'\n';
            m_stream.Write(msg_with_EoLn, 0, msg_with_EoLn.Length);
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
            content_encoded[EoDidx + 0] = (byte)'\r';
            content_encoded[EoDidx + 1] = (byte)'\n';
            content_encoded[EoDidx + 2] = (byte)'.';
            content_encoded[EoDidx + 3] = (byte)'\r';
            content_encoded[EoDidx + 4] = (byte)'\n';
            m_stream.Write(content_encoded, 0, content_encoded.Length);
        }
    };
}
