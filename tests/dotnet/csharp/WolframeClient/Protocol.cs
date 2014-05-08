using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

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

            while (0 < (idx = Array.IndexOf(msg, '\n', idx)))
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

            while (0 < (nextidx = Array.IndexOf(msg, '\n', idx)))
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

            while (0 < (idx = Array.IndexOf(msg, '\n', idx)))
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

            while (0 < (nextidx = Array.IndexOf(msg, '\n', idx)))
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
            public int pos { get; set; }
            public int size { get; set; }
            public byte[] ar { get; set; }

            public Buffer( int initsize)
            {
                pos = 0;
                size = 0;
                ar = new byte[initsize];
            }

            private void Grow()
            {
                if (ar == null || ar.Length == 0)
                {
                    ar = new byte[4096];
                    size = 0;
                    pos = 0;
                }
                else
                {
                    byte[] new_ar = null;
                    if (pos > ar.Length / 2)
                    {
                        new_ar = new byte[ar.Length];
                    }
                    else
                    {
                        new_ar = new byte[ar.Length * 2];
                    }
                    Array.Copy(ar, pos, new_ar, 0, size - pos);
                    size = size - pos;
                    pos = 0;
                }
            }

            private byte[] GetMessageFromBuffer(int idx, int nextidx)
            {
                byte[] msg = new byte[idx-pos];
                Array.Copy(ar, pos, msg, 0, idx-pos);
                pos = nextidx;
                return msg;
            }

            private int FetchData(Stream src, int idx)
            {
                if (size >= ar.Length)
                {
                    int searchpos = idx - pos;
                    Grow();
                    idx = searchpos;
                }
                int nof_bytes_read = src.Read(ar, size, ar.Length - size);
                if (nof_bytes_read <= 0) return -1;
                size = size + nof_bytes_read;
                return idx;
            }

            public byte[] FetchLine(Stream src)
            {
                int idx = Array.IndexOf(ar, '\n',pos,size-pos);
                while (idx == -1)
                {
                    int newidx = FetchData(src, idx);
                    if (newidx == -1) return null;
                    idx = newidx;
                    idx = Array.IndexOf(ar, '\n', idx, size - idx);
                }
                if (idx > pos && ar[idx - 1] == '\r')
                {
                    return GetMessageFromBuffer(idx - 1, idx + 1);
                }
                else
                {
                    return GetMessageFromBuffer(idx, idx + 1);
                }
            }

            public byte[] FetchContent(Stream src)
            {
                int idx = Array.IndexOf(ar, '\n', pos, size - pos);
                while (idx >= pos)
                {
                    int nextidx = -1;
                    if (size >= idx + 2)
                    {
                        if (ar[idx + 1] == '.')
                        {
                            if (ar[idx + 2] == '\n')
                            {
                                nextidx = idx + 3;
                            }
                            else if (ar[idx + 2] == '\r')
                            {
                                if (size >= idx + 3)
                                {
                                    if (ar[idx + 3] == '\n')
                                    {
                                        nextidx = idx + 4;
                                    }
                                }
                                else
                                {
                                    int newidx = FetchData(src, idx);
                                    if (newidx == -1) return null;
                                    idx = newidx;
                                }
                            }
                        }
                    }
                    else
                    {
                        int newidx = FetchData(src, idx);
                        if (newidx == -1) return null;
                        idx = newidx;
                    }
                    if (nextidx > 0)
                    {
                        return Protocol.UnescapeLFdot(GetMessageFromBuffer(idx, nextidx));
                    }
                    idx = Array.IndexOf(ar, '\n', idx+1, size - idx-1);
                }
                return null;
            }

            public bool HasData()
            {
                return (pos < size);
            }
        };
    }
}
