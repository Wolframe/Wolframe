using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
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
        private string m_host;
        private IPAddress[] m_addresses;
        private int m_port;
        private TcpClient m_client;
        private NetworkStream m_stream;
        private object m_streamLock;
        private Protocol.Buffer m_buffer;
        private ConcurrentQueue<byte[]> m_writequeue;
        private volatile bool m_waitwrite;
        private object m_waitwriteLock;
        private volatile bool m_waitread;
        private object m_waitreadLock;
        private volatile bool m_eofread;

        private void EndWriteCallback(IAsyncResult ev)
        {
            lock (m_streamLock)
            {
                if (m_stream.CanWrite)
                {
                    m_stream.EndWrite(ev);
                }
            }
            byte[] msg = null;
            bool doBeginWrite = false;

            lock (m_waitwriteLock)
            {
                doBeginWrite = m_writequeue.TryDequeue(out msg);
                m_waitwrite = doBeginWrite;
            }
            lock (m_streamLock)
            {
                if (doBeginWrite && m_stream.CanWrite)
                {
                    m_stream.BeginWrite(msg, 0, msg.Length, EndWriteCallback, null);
                }
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
                lock (m_streamLock)
                {
                    if (m_stream.CanWrite)
                    {
                        m_stream.BeginWrite(msg, 0, msg.Length, EndWriteCallback, null);
                    }
                }
            }
        }

        private void EndReadCallback(IAsyncResult ev)
        {
            int nof_bytes_read;
            lock (m_streamLock)
            {
                if (m_stream.CanRead)
                {
                    nof_bytes_read = m_stream.EndRead(ev);
                }
                else
                {
                    nof_bytes_read = 0;
                }
            }
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
                lock (m_streamLock)
                {
                    if (m_stream.CanRead)
                    {
                        m_stream.BeginRead(buf, 0, buf.Length, EndReadCallback, buf);
                    }
                }
            }
        }

/* PUBLIC METHODS: */
        public Connection( string host_, int port_)
        {
            m_host = host_;
            m_addresses = null;
            m_port = port_;
            m_client = new TcpClient();
            m_stream = null;
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
            m_addresses = Dns.GetHostAddresses( m_host);
            string err = null;
            int ii = 0;
            for (; ii<m_addresses.Length; ++ii)
            {
   	            try
		        {
                    m_client.Connect( m_addresses[ii], m_port);
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
            m_stream = m_client.GetStream();
            m_buffer = new Protocol.Buffer(m_stream, 4096, IssueReadRequest);
            IssueReadRequest();
        }

        public void Close()
        {
            m_buffer.Close();
            lock (m_streamLock)
            {
                m_stream.Close();
            }
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
