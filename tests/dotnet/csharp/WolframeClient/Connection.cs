using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;
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

        public Connection( string ipadr, int port)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_buffer = new Protocol.Buffer( 4096);
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
            }
            m_stream.Close();
        }

        public byte[] ReadLine()
        {
            return m_buffer.FetchLine(m_stream);
        }

        public byte[] ReadContent()
        {
            return m_buffer.FetchContent(m_stream);
        }

        public void WriteLine(string ln)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);

            Byte[] msg = utf8.GetBytes(msgstr);
            byte[] msg_with_EoLn = new byte[msg.Length + 2];
            msg_with_EoLn[msg.Length + 0] = (byte)'\r';
            msg_with_EoLn[msg.Length + 1] = (byte)'\n';
            m_stream.Write(msg_with_EoLn, 0, msg_with_EoLn.Length);
        }

        public void WriteRequest(string command, byte[] content)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);
            byte[] hdr = utf8.GetBytes("REQUEST " + command + "\r\n");
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
