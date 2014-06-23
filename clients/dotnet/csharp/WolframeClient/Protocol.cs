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
/* PUBLIC METHODS: */
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

        public static bool IsEqual( string a1, string a2)
        {
           return String.Equals( a1, a2, StringComparison.OrdinalIgnoreCase);
        }
    }
}
