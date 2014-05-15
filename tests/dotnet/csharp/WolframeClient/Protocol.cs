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

            // Count number of LFdot to map to LF:
            while (0 < (idx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                idx = idx + 1;
                if (msg.Length > idx && msg[ idx] == '.')
                {
                    //... for each LF dot sequence we need one byte less in the result
                    size = size - 1;
                }
            }
            if (size == msg.Length)
            {
                //... nothing to do because no LFdot sequences found
                return msg;
            }
            // Allocate result message with fitting size:
            rt = new byte[size];

            // Map all LFdot to LF:
            idx = 0;
            while (0 < (nextidx = Array.IndexOf(msg, (byte)'\n', idx)))
            {
                int linesize = nextidx - idx + 1/*incl LF*/;
                Array.Copy(msg, idx, rt, rtidx, linesize);
                rtidx = rtidx + linesize;
                idx = nextidx + 1;

                if (msg.Length >= idx)
                {
                    if (msg[idx] == '.')
                    {
                        //... ignore first dot after LF
                        idx = idx + 1;
                    }
                }
            }
            // Copy rest chunk without terminating LF:
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

        public class ReadChunk
        {
            public byte[] ar { get; set; }
            public int size { get; set; }
        };

        public class Buffer
        {
            public delegate void IssueReadRequest();

            private int m_pos;
            private int m_scanidx;
            private int m_size;
            private byte[] m_ar;
            private object m_arLock;
            private bool m_endOfData;
            private IssueReadRequest m_issueReadRequestCallback;
            private AutoResetEvent m_readqueue_signal;
            private Queue<ReadChunk> m_readqueue;

            public Buffer(Stream src_, int initsize, IssueReadRequest issueReadRequestCallback_)
            {
                m_pos = 0;
                m_scanidx = 0;
                m_size = 0;
                m_ar = new byte[initsize];
                m_arLock = new object();
                m_endOfData = false;
                m_issueReadRequestCallback = issueReadRequestCallback_;
                m_readqueue_signal = new AutoResetEvent(false);
                m_readqueue = new Queue<ReadChunk>();
            }

            public void Close()
            {
                m_endOfData = true;
                m_readqueue_signal.Set();
            }

            private void Grow()
            {
                if (m_ar == null || m_ar.Length == 0)
                {
                    m_ar = new byte[4096];
                    m_size = 0;
                    m_pos = 0;
                    m_scanidx = 0;
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
                    m_scanidx -= m_pos;
                    m_pos = 0;
                    m_ar = new_ar;
                }
            }

            private void EatReadChunk(ReadChunk rc)
            {
                lock (m_arLock)
                {
                    while (m_size + rc.size > m_ar.Length)
                    {
                        Grow();
                    }
                    Array.Copy(rc.ar, 0, m_ar, m_size, rc.size);
                    m_size += rc.size;
                }
            }

            private byte[] GetMessageFromBuffer( int endOfDataMarkerSize)
            {
                lock (m_arLock)
                {
                    byte[] msg = new byte[m_scanidx - m_pos];
                    Array.Copy(m_ar, m_pos, msg, 0, m_scanidx - m_pos);
                    m_scanidx += endOfDataMarkerSize;
                    m_pos = m_scanidx;
                    return msg;
                }
            }

            private bool FetchReadQueueElem()
            {
                    bool rt = false;
                    try
                    {
                        ReadChunk rc = m_readqueue.Dequeue();
                        if (rc.ar == null)
                        {
                            m_endOfData = true;
                        }
                        else
                        {
                            EatReadChunk(rc);
                            rt = true;
                        }
                    }
                    catch (InvalidOperationException)
                    {}
                    return rt;
            }

            private bool FetchData()
            {
                int nofChunksRead = 0;
                while (!m_endOfData)
                {
                    if (FetchReadQueueElem())
                    {
                        nofChunksRead += 1;
                    }
                    else if (nofChunksRead != 0)
                    {
                        return true;
                    }
                    else
                    {
                        m_issueReadRequestCallback();
                        m_readqueue_signal.WaitOne();
                    }
                }
                return false/*EOF*/;
            }

            private bool FindEndOfLine()
            {
                do
                {
                    lock (m_arLock)
                    {
                        int idx = Array.IndexOf(m_ar, (byte)'\n', m_scanidx, m_size - m_scanidx);
                        if (idx >= 0)
                        {
                            m_scanidx = idx;
                            return true;
                        }
                    }
                } while (FetchData());

                return false;
            }

            private int ScanEndOfData()
            {
                int eodsize = -1;
                lock (m_arLock)
                {
                    if (m_size >= m_scanidx + 2)
                    {
                        if (m_ar[m_scanidx + 1] == (byte)'.')
                        {
                            if (m_ar[m_scanidx + 2] == (byte)'\n')
                            {
                                eodsize = 3;
                            }
                            else if (m_ar[m_scanidx + 2] == (byte)'\r')
                            {
                                if (m_size >= m_scanidx + 3)
                                {
                                    if (m_ar[m_scanidx + 3] == (byte)'\n')
                                    {
                                        eodsize = 4;
                                    }
                                }
                                else
                                {
                                    eodsize = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        eodsize = 0;
                    }
                }
                return eodsize;
            }

/* PUBLIC METHODS: */
            public bool HasData()
            {
                lock (m_arLock)
                {
                    if (m_pos < m_size) return true;
                }
                return FetchReadQueueElem();
            }

            public byte[] FetchLine()
            {
                if (FindEndOfLine())
                {
                    int eolnsize = 1;
                    lock (m_arLock)
                    {
                        if (m_scanidx > m_pos && m_ar[m_scanidx - 1] == '\r')
                        {
                            m_scanidx -= 1;
                            eolnsize += 1;
                        }
                    }
                    return GetMessageFromBuffer(eolnsize);
                }
                return null;
            }

            public byte[] FetchContent()
            {
                while (FindEndOfLine())
                {
                    int eodsize;
                    do
                    {
                        eodsize = ScanEndOfData();
                    }
                    while (eodsize == 0 && FetchData());

                    if (eodsize == 0)
                    {
                        return null;
                    }
                    else if (eodsize == -1)
                    {
                        lock (m_arLock)
                        {
                            m_scanidx++;
                        }
                    }
                    else
                    {
                        return Protocol.UnescapeLFdot( GetMessageFromBuffer( eodsize));
                    }
                }
                return null;
            }

            public void PushReadChunk(ReadChunk rc)
            {
                m_readqueue.Enqueue(rc);
                m_readqueue_signal.Set();
            }
        };
    }
}
