using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.Security.Authentication;
using System.Collections;
using System.Threading;
using System.Collections.Generic;
using System.Collections.Concurrent;
using WolframeClient;

namespace WolframeClient
{
    public class Connection
        : ConnectionInterface
    {
        public class Configuration
        {
            public string host { get; set; }
            public int port { get; set; }
            public string sslcert { get; set; }
            public string password { get; set; }

            public Configuration()
            {
                host = "localhost";
                port = 7661;
                sslcert = null;
                password = null;
            }
        };

        private Configuration m_config;
        private IPAddress[] m_addresses;
        private TcpClient m_client;
        private NetworkStream m_stream;
        private SslStream m_sslstream;
        private object m_streamLock;
        private Protocol.Buffer m_buffer;
        private ConcurrentQueue<byte[]> m_writequeue;
        private volatile bool m_waitwrite;
        private object m_waitwriteLock;
        private volatile bool m_waitread;
        private object m_waitreadLock;
        private volatile bool m_eofread;

        private void EndWrite(IAsyncResult ev)
        {
            lock (m_streamLock)
            {
                if (m_sslstream != null)
                {
                    if (m_sslstream.CanWrite)
                    {
                        m_sslstream.EndWrite(ev);
                    }
                }
                else
                {
                    if (m_stream.CanWrite)
                    {
                        m_stream.EndWrite(ev);
                    }
                }
            }
        }

        private int EndRead(IAsyncResult ev)
        {
            lock (m_streamLock)
            {
                if (m_sslstream != null)
                {
                    if (m_sslstream.CanRead)
                    {
                        return m_sslstream.EndRead(ev);
                    }
                }
                else
                {
                    if (m_stream.CanRead)
                    {
                        return m_stream.EndRead(ev);
                    }
                }
            }
            return 0;
        }

        private void BeginWrite(byte[] msg_, int offset_, int size_, AsyncCallback callback_, object state_)
        {
            lock (m_streamLock)
            {
                if (m_sslstream != null)
                {
                    if (m_sslstream.CanWrite)
                    {
                        m_sslstream.BeginWrite(msg_, offset_, msg_.Length, callback_, state_);
                    }
                }
                else
                {
                    if (m_stream.CanWrite)
                    {
                        m_stream.BeginWrite(msg_, offset_, msg_.Length, callback_, state_);
                    }
                }
            }
        }

        private void BeginRead(byte[] msg_, int offset_, int size_, AsyncCallback callback_, object state_)
        {
            lock (m_streamLock)
            {
                if (m_sslstream != null)
                {
                    if (m_sslstream.CanRead)
                    {
                        m_sslstream.BeginRead(msg_, offset_, msg_.Length, callback_, state_);
                    }
                }
                else
                {
                    if (m_stream.CanRead)
                    {
                        m_stream.BeginRead(msg_, offset_, msg_.Length, callback_, state_);
                    }
                }
            }
        }

        private void CloseStream()
        {
            lock (m_streamLock)
            {
                if (m_sslstream != null)
                {
                    m_sslstream.Close();
                }
                else
                {
                    m_stream.Close();
                }
            }
        }

        public static bool ValidateServerCertificate(
              object sender,
              X509Certificate certificate,
              X509Chain chain,
              SslPolicyErrors sslPolicyErrors)
        {
            return true;
        }

        private void OpenStream()
        {
            if (m_config.sslcert != null)
            {
                m_stream = null;
                // Create an SSL stream that will close the client's stream.
                m_sslstream = new SslStream(m_client.GetStream(),
                    false,
                    new RemoteCertificateValidationCallback(ValidateServerCertificate),
                    null
                    );
                m_sslstream.AuthenticateAsClient(m_config.sslcert/*server name == certificate name*/);
            }
            else
            {
                m_stream = m_client.GetStream();
                m_sslstream = null;
            }
        }

        private void EndWriteCallback(IAsyncResult ev)
        {
            EndWrite(ev);
            byte[] msg = null;
            bool doBeginWrite = false;

            lock (m_waitwriteLock)
            {
                doBeginWrite = m_writequeue.TryDequeue(out msg);
                m_waitwrite = doBeginWrite;
            }
            if (doBeginWrite)
            {
                BeginWrite(msg, 0, msg.Length, EndWriteCallback, null);
            }
        }

        private void FlushWriteQueue()
        {
            byte[] msg = null;
            bool doBeginWrite = false;
            lock (m_waitwriteLock)
            {
                if (!m_waitwrite)
                {
                    doBeginWrite = m_writequeue.TryDequeue(out msg);
                    m_waitwrite = doBeginWrite;
                }
            }
            if (doBeginWrite)
            {
                BeginWrite(msg, 0, msg.Length, EndWriteCallback, null);
            }
        }

        private void EndReadCallback(IAsyncResult ev)
        {
            int nof_bytes_read = EndRead(ev);
            lock (m_waitreadLock)
            {
                m_waitread = false;
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
        }

        public void IssueReadRequest()
        {
            int readsize = 2048;
            int available = m_client.Available;
            if (available > readsize)
            {
                readsize = available;
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
                BeginRead(buf, 0, buf.Length, EndReadCallback, buf);
            }
        }


/* PUBLIC METHODS: */
        public Connection( Configuration config_)
        {
            m_config = config_;
            m_addresses = null;
            m_client = new TcpClient();
            m_stream = null;
            m_sslstream = null;
            m_streamLock = new object();
            m_buffer = null;
            m_writequeue = new ConcurrentQueue<byte[]>();
            m_waitwrite = false;
            m_waitwriteLock = new object();
            m_waitread = false;
            m_waitreadLock = new object();
            m_eofread = false;
        }

        public void Connect()
        {
            m_addresses = Dns.GetHostAddresses( m_config.host);
            string err = null;
            int ii = 0;
            for (; ii<m_addresses.Length; ++ii)
            {
   	            try
		        {
                    m_client.Connect(m_addresses[ii], m_config.port);
                    break;
                }
                catch (Exception e)
                {
                    string errmsg = "Could not connect to " + m_addresses[ii].ToString() + ": " + e.Message;
                    if (err == null)
                    {
                        err = errmsg;
                    }
                    else
                    {
                        err = err + "; " + errmsg;
                    }
                }
            }
            if (ii == m_addresses.Length)
            {
                if (err == null)
                {
                    err = "Could not resolve address";
                }
                throw new Exception(err);
            }
            OpenStream();
            m_buffer = new Protocol.Buffer( 4096, IssueReadRequest);
            IssueReadRequest();
        }

        public void Close()
        {
            m_buffer.Close();
            CloseStream();
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
