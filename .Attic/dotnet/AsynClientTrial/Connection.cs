using System;
using System.Text;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Collections;

namespace WolframeClient
{
    class Connection
    {
        private IPAddress m_address;
        private int m_port;
        private TcpClient m_client;
        private NetworkStream m_stream;
        public enum Error { NoError = 0, ConnectionFailed, WriteFailed, ReadFailed, ConnectionClosed };
        private Error m_error;
        public enum State { Init = 0, Connecting, Connected, WaitWrite };
        private State m_state;
        private byte[] m_readbuf;
        private object m_readbuflock;
        private byte[] m_writebuf;
        private object m_writebuflock;

        public delegate void ProcessErrorDelegate( Error err);
        public delegate void ProcessMessageDelegate( byte[] msg);

        ProcessErrorDelegate m_processError;
        ProcessMessageDelegate m_processMessage;

        public Connection( string ipadr, int port, ProcessErrorDelegate processError, ProcessMessageDelegate processMessage)
        {
            m_address = IPAddress.Parse( ipadr);
            m_port = port;
            m_client = new TcpClient();
            m_stream = null;
            m_error = Error.NoError;
            m_readbuf = null;
            m_writebuf = null;
            m_processError = processError;
            m_processMessage = processMessage;
            m_state = State.Init;
            m_readbuflock = new object();
            m_writebuflock = new object();
        }

        public void ConnectCallback( IAsyncResult result)
        {
            try
            {
                m_client.EndConnect( result);
                m_stream = m_client.GetStream();
                m_processMessage( null);
                m_state = State.Connected;
            }
            catch
            {
                m_error = Error.ConnectionFailed;
                m_processError( m_error);
            }
        }

        public void WriteCallback( IAsyncResult result)
        {
            try
            {
                m_stream.EndWrite(result);
                lock (m_writebuflock)
                {
                    if (m_writebuf != null)
                    {
                        m_stream.BeginWrite(m_writebuf, 0, m_writebuf.Length, WriteCallback, null);
                        m_writebuf = null;
                    }
                    else if (m_state == State.WaitWrite)
                    {
                        m_state = State.Connected;
                    }
                }
            }
            catch
            {
                m_error = Error.WriteFailed;
                m_processError( m_error);
            }
        }

        public void pushReceivedBuffer( byte[] msgbuf)
        {
            lock (m_readbuflock)
            {
                if (m_readbuf != null)
                {
                    byte[] joinbuf = new byte[m_readbuf.Length + msgbuf.Length];
                    Array.Copy(m_readbuf, 0, joinbuf, 0, m_readbuf.Length);
                    Array.Copy(msgbuf, 0, joinbuf, m_readbuf.Length, msgbuf.Length);
                    m_readbuf = joinbuf;
                }
                else
                {
                    m_readbuf = msgbuf;
                }
            }
        }

        private byte[] getMessageFromBuffer( int idx, int nextidx)
        {
            byte[] msg = new byte[idx];
            Array.Copy( m_readbuf, 0, msg, 0, idx);
            if (m_readbuf.Length > nextidx)
            {
                byte[] rest_buffer = new byte[ m_readbuf.Length - nextidx];
                Array.Copy( m_readbuf, nextidx, rest_buffer, 0, rest_buffer.Length);
                m_readbuf = rest_buffer;
            }
            else
            {
                m_readbuf = null;
            }
            return msg;
        }

        public byte[] fetchContentFromBuffer()
        {
            lock (m_readbuflock)
            {
                int idx = -1;
                int nextidx = 0;

                // Find LF . CRLF or LF . LF marking the end of message:
                while (nextidx == 0 && 0 < (idx = Array.IndexOf(m_readbuf, '\n', idx + 1)))
                {
                    if (m_readbuf.Length >= idx + 2)
                    {
                        if (m_readbuf[idx + 1] == '.')
                        {
                            if (m_readbuf[idx + 2] == '\n')
                            {
                                nextidx = idx + 3;
                            }
                            else if (m_readbuf[idx + 2] == '\r')
                            {
                                if (m_readbuf.Length >= idx + 3 && m_readbuf[idx + 3] == '\n')
                                {
                                    nextidx = idx + 4;
                                }
                            }
                        }
                    }
                }
                // If available return message with unescaped LF . sequences:
                if (nextidx != 0)
                {
                    return Protocol.unescapeLFdot(getMessageFromBuffer(idx, nextidx));
                }
                return null;
            }
        }

        public byte[] fetchLineFromBuffer()
        {
            lock (m_readbuflock)
            {
                int idx = Array.IndexOf(m_readbuf, '\n');
                if (idx < 0) return null;
                if (idx > 0 && m_readbuf[idx - 1] == '\r')
                {
                    return getMessageFromBuffer(idx - 1, idx + 1);
                }
                else
                {
                    return getMessageFromBuffer(idx, idx + 1);
                }
            }
        }

        public enum FetchType {Line,Content};
        public void FetchData(IAsyncResult result, FetchType what)
        {
            int readlen = 0;
            try
            {
                readlen = m_stream.EndRead(result);
                if (readlen == 0)
                {
                    m_error = Error.ConnectionClosed;
                    m_processError( m_error);
                }
                else
                {
                    pushReceivedBuffer( result.AsyncState as byte[]);
                    byte[] msg = null;
                    switch (what)
                    {
                        case FetchType.Line:
                            msg = fetchLineFromBuffer();
                            break;
                        case FetchType.Content:
                            msg = fetchContentFromBuffer();
                            break;
                    }
                    if (msg != null)
                    {
                        m_processMessage(msg);
                    }
                    else
                    {
                        byte[] netbuffer = new byte[ m_client.ReceiveBufferSize];
                        m_stream.BeginRead( netbuffer, 0, netbuffer.Length, ReadLineCallback, netbuffer);
                    }
                }
            }
            catch
            {
                m_error = Error.ReadFailed;
            }
		}

        public void ReadLineCallback(IAsyncResult result)
        {
            FetchData(result, FetchType.Line);
        }

        public void ReadContentCallback(IAsyncResult result)
        {
            FetchData(result, FetchType.Content);
        }

        public void Connect()
        {
            m_client.BeginConnect( m_address, m_port, ConnectCallback, null);
            m_state = State.Connecting;
        }

        public void Close()
        {
            switch (m_state)
            {
                case State.WaitWrite:
                    m_stream.Close();
                    m_client.Close();
                    break;
                case State.Connected:
                    m_stream.Close();
                    m_client.Close();
                    break;
                case State.Connecting:
                    m_client.Close();
                    break;
                case State.Init:
                    break;
            }
        }

        public void IssueReadLine()
        {
            byte[] line = fetchLineFromBuffer();
            if (line != null)
            {
                m_processMessage(line);
            }
            else
            {
                byte[] netbuffer = new byte[m_client.ReceiveBufferSize];
                m_stream.BeginRead(netbuffer, 0, netbuffer.Length, ReadLineCallback, netbuffer);
            }
        }

        public void IssueReadContent()
        {
            byte[] msg = fetchContentFromBuffer();
            if (msg != null)
            {
                m_processMessage(msg);
            }
            else
            {
                byte[] netbuffer = new byte[m_client.ReceiveBufferSize];
                m_stream.BeginRead(netbuffer, 0, netbuffer.Length, ReadContentCallback, netbuffer);
            }
        }

        public void WriteBytes( byte[] bytes)
        {
            lock (m_writebuflock)
            {
                if (m_state == State.WaitWrite)
                {
                    if (m_writebuf != null)
                    {
                        byte[] joinbuf = new byte[m_writebuf.Length + bytes.Length];
                        Array.Copy(m_writebuf, 0, joinbuf, 0, m_writebuf.Length);
                        Array.Copy(bytes, 0, joinbuf, m_writebuf.Length, bytes.Length);
                        m_writebuf = joinbuf;
                    }
                    else
                    {
                        m_writebuf = bytes;
                    }
                    return;
                }
            }
            m_stream.BeginWrite( bytes, 0, bytes.Length, WriteCallback, null);
        }

        public void WriteLine( string msgstr)
        {
            UTF8Encoding utf8 = new UTF8Encoding( false/*no BOM*/, true/*throw if input illegal*/);

            Byte[] msg = utf8.GetBytes( msgstr);
            byte[] msg_with_EoLn = new byte[ msg.Length + 2];
            msg_with_EoLn[ msg.Length + 0] = (byte)'\r';
            msg_with_EoLn[ msg.Length + 1] = (byte)'\n';
            WriteBytes( msg_with_EoLn);
        }

        public void WriteContent( byte[] bytes)
        {
            byte[] content = Protocol.escapeLFdot(bytes);
            byte[] content_with_EoD = new byte[ content.Length + 5];
            content_with_EoD[ content.Length + 0] = (byte)'\r';
            content_with_EoD[ content.Length + 1] = (byte)'\n';
            content_with_EoD[ content.Length + 2] = (byte)'.';
            content_with_EoD[ content.Length + 3] = (byte)'\r';
            content_with_EoD[ content.Length + 4] = (byte)'\n';
            WriteBytes( content_with_EoD);
        }
    };
}
