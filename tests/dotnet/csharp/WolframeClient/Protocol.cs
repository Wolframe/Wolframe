using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;

namespace WolframeClient
{
    class Protocol
    {
        public static byte[] EscapeLFdot(byte[] msg)
        {
            byte[] rt = null;
            int rtidx = 0;
            int idx = 0;
            int nextidx = 0;
            int size = msg.Length;

            while (0 < (idx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                if (msg.Length > idx && msg[idx + 1] == '.')
                {
                    size = size + 1;
                }
            }
            if (size == msg.Length)
            {
                return msg;
            }
            rt = new byte[size];

            while (0 < (nextidx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                Array.Copy(msg, idx, rt, rtidx, nextidx - idx + 1);
                rtidx = rtidx + nextidx - idx + 1;
                idx = nextidx + 1;

                if (msg.Length >= idx)
                {
                    if (msg[idx] == '.')
                    {
                        rt[rtidx] = (byte)'.';
                        rtidx = rtidx + 1;
                    }
                }
            }
            Array.Copy(msg, idx, rt, rtidx, msg.Length - idx);
            return rt;
        }

        public static byte[] UnescapeLFdot(byte[] msg)
        {
            byte[] rt = null;
            int rtidx = 0;
            int idx = 0;
            int nextidx = 0;
            int size = msg.Length;

            while (0 < (idx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                if (msg.Length > idx && msg[idx + 1] == '.')
                {
                    size = size - 1;
                }
            }
            if (size == msg.Length)
            {
                return msg;
            }
            rt = new byte[size];

            while (0 < (nextidx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                Array.Copy(msg, idx, rt, rtidx, nextidx - idx + 1);
                rtidx = rtidx + nextidx - idx + 1;
                idx = nextidx + 1;

                if (msg.Length >= idx)
                {
                    if (msg[idx] == '.')
                    {
                        idx = idx + 1;
                    }
                }
            }
            Array.Copy(msg, idx, rt, rtidx, msg.Length - idx);
            return rt;
        }

        public static bool IsCommand(string cmd, byte[] msg)
        {
            if (cmd.Length > msg.Length) return false;
            if (msg.Length > cmd.Length && msg[cmd.Length] != (byte)' ') return false;
            int ii = 0;
            for (; ii < cmd.Length; ++ii) if (msg[ii] != (byte)cmd[ii]) return false;
            return true;
        }

        public static string CommandArg(string cmd, byte[] msg)
        {
            return Encoding.UTF8.GetString(msg, cmd.Length + 1, msg.Length - cmd.Length -1);
        }

        public class Buffer
        {
            private int m_pos;
            private int m_size;
            private byte[] m_ar;
            private string m_lasterror;
            private bool m_eod;
            private Stream m_src;
            AutoResetEvent m_dataready;

            public Buffer( Stream src_, int initsize)
            {
                m_pos = 0;
                m_size = 0;
                m_ar = new byte[initsize];
                m_lasterror = null;
                m_eod = false;
                m_src = src_;
                m_dataready = new AutoResetEvent(false); ;
            }

            private void Grow()
            {
                if (m_ar == null || m_ar.Length == 0)
                {
                    m_ar = new byte[4096];
                    m_size = 0;
                    m_pos = 0;
                }
                else
                {
                    byte[] new_ar = null;
                    if (m_pos > m_ar.Length / 2)
                    {
                        new_ar = new byte[m_ar.Length];
                    }
                    else
                    {
                        new_ar = new byte[m_ar.Length * 2];
                    }
                    Array.Copy(m_ar, m_pos, new_ar, 0, m_size - m_pos);
                    m_size = m_size - m_pos;
                    m_pos = 0;
                }
            }

            private byte[] GetMessageFromBuffer(int idx, int nextidx)
            {
                byte[] msg = new byte[idx-m_pos];
                Array.Copy(m_ar, m_pos, msg, 0, idx-m_pos);
                m_pos = nextidx;
                return msg;
            }

            private void EndRead(IAsyncResult ev)
            {
                m_lasterror = null;
                try
                {
                    int nof_bytes_read = m_src.EndRead(ev);
                    if (nof_bytes_read > 0)
                    {
                        m_size = m_size + nof_bytes_read;
                    }
                    else if (nof_bytes_read == 0)
                    {
                        m_eod = true;
                    }
                }
                catch (IOException e)
                {
                    m_lasterror = e.Message;
                }
                m_dataready.Set();
            }

            private int FetchData( int idx)
            {
                m_lasterror = null;
                if (m_size >= m_ar.Length)
                {
                    int searchpos = idx - m_pos;
                    Grow();
                    idx = searchpos;
                }
                var ev = m_src.BeginRead(m_ar, m_size, m_ar.Length - m_size, EndRead, null);
                if (!ev.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(2)))
                {
                    throw new Exception("Timeout in socket stream read");
                }
                m_dataready.WaitOne();
                if (m_lasterror != null)
                {
                    string err = m_lasterror;
                    m_lasterror = null;
                    throw new Exception("Read failed: " + m_lasterror);
                }
                if (m_eod)
                {
                    return -1;
                }
                return idx;
            }

            public void IssueRead(AutoResetEvent signal_)
            {
                var ev = m_src.BeginRead(m_ar, m_size, m_ar.Length - m_size,
                                        delegate(IAsyncResult r)
                                        {
                                            m_size = m_size + m_src.EndRead(r);
                                            AutoResetEvent sg = r.AsyncState as AutoResetEvent;
                                            sg.Set();
                                        }, signal_);
            }

            public byte[] FetchLine()
            {
                int idx = Array.IndexOf(m_ar, (byte)'\n',m_pos,m_size-m_pos);
                int newidx = m_size;
                while (idx == -1)
                {
                    newidx = FetchData( newidx);
                    if (newidx == -1) return null;
                    idx = Array.IndexOf(m_ar, (byte)'\n', newidx, m_size - newidx);
                }
                if (idx > m_pos && m_ar[idx - 1] == '\r')
                {
                    return GetMessageFromBuffer(idx - 1, idx + 1);
                }
                else
                {
                    return GetMessageFromBuffer(idx, idx + 1);
                }
            }

            public byte[] FetchContent()
            {
                int idx = Array.IndexOf(m_ar, (byte)'\n', m_pos, m_size - m_pos);
                while (idx >= m_pos)
                {
                    int nextidx = -1;
                    if (m_size >= idx + 2)
                    {
                        if (m_ar[idx + 1] == (byte)'.')
                        {
                            if (m_ar[idx + 2] == (byte)'\n')
                            {
                                nextidx = idx + 3;
                            }
                            else if (m_ar[idx + 2] == (byte)'\r')
                            {
                                if (m_size >= idx + 3)
                                {
                                    if (m_ar[idx + 3] == (byte)'\n')
                                    {
                                        nextidx = idx + 4;
                                    }
                                }
                                else
                                {
                                    int newidx = FetchData(idx);
                                    if (newidx == -1) return null;
                                    idx = newidx;
                                }
                            }
                        }
                    }
                    else
                    {
                        int newidx = FetchData(idx);
                        if (newidx == -1) return null;
                        idx = newidx;
                    }
                    if (nextidx > 0)
                    {
                        return Protocol.UnescapeLFdot(GetMessageFromBuffer(idx, nextidx));
                    }
                    idx = Array.IndexOf(m_ar, (byte)'\n', idx+1, m_size - idx-1);
                }
                return null;
            }

            public bool HasData()
            {
                return (m_pos < m_size);
            }
        };
    }
}
