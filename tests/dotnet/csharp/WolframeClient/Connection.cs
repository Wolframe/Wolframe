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
        private AutoResetEvent m_signal;

        public Connection(string ipadr, int port, AutoResetEvent signal)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_buffer = null;
            m_readRequestIssued = false;
            m_signal = signal;
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
            return m_buffer.FetchLine();
        }

        public byte[] ReadContent()
        {
            if (m_buffer == null) throw new Exception("read content failed");
            m_readRequestIssued = false;
            return m_buffer.FetchContent();
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
            msg[EoDidx + 0] = (byte)'\r';
            msg[EoDidx + 1] = (byte)'\n';
            msg[EoDidx + 2] = (byte)'.';
            msg[EoDidx + 3] = (byte)'\r';
            msg[EoDidx + 4] = (byte)'\n';
            m_stream.Write( msg, 0, msg.Length);
        }
    };
}
