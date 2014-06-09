using System;
using System.Text;
using System.IO;
using System.Net;
using System.Security;
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
            public SecureString password { get; set; }
            public bool validatecert { get; set; }

            public Configuration()
            {
                host = "localhost";
                port = 7661;
                sslcert = null;
                password = null;
                validatecert = true;
            }
        };

        private Configuration m_config;
        private IPAddress[] m_addresses;
        private TcpClient m_client;
        private NetworkStream m_stream;
        private SslStream m_sslstream;
        private byte[] m_readbuffer;
        private string m_lasterror;

        private void Write(byte[] msg_, int offset_, int size_)
        {
            if (m_sslstream != null)
            {
                if (m_sslstream.CanWrite)
                {
                    m_sslstream.Write( msg_, offset_, size_);
                }
            }
            else if (m_stream != null)
            {
                if (m_stream.CanWrite)
                {
                    m_stream.Write( msg_, offset_, size_);
                }
            }
        }

        enum DetectEoD { NeedMoreData, Yes, No };
        private DetectEoD doDetectEoD(ref byte[] ar, int ofs, int size, out int next)
        {
            next = ofs;
            if (ofs < size && ar[ofs] == (byte)'\n')
            {
                if (ofs + 1 >= size) return DetectEoD.NeedMoreData;
                if (ar[ofs + 1] == (byte)'.')
                {
                    if (ofs + 2 >= size) return DetectEoD.NeedMoreData;
                    if (ar[ofs + 2] == (byte)'\r')
                    {
                        if (ofs + 3 >= size) return DetectEoD.NeedMoreData;
                        if (ar[ofs + 3] == (byte)'\n')
                        {
                            next = ofs + 4;
                            return DetectEoD.Yes;
                        }
                        else
                        {
                            return DetectEoD.No;
                        }
                    }
                    else if (ar[ofs + 2] == (byte)'\n')
                    {
                        next = ofs + 3;
                        return DetectEoD.Yes;
                    }
                    else
                    {
                        return DetectEoD.No;
                    }
                }
                else
                {
                    return DetectEoD.No;
                }
            }
            return DetectEoD.No;
        }

        private void CopyToReadBuffer( byte[] ar_, int offset_, int size_)
        {
            int readbuffer_size = 0;
            if (m_readbuffer != null) readbuffer_size = m_readbuffer.Length;
            byte[] new_readbuffer = new byte[ readbuffer_size + size_];
            if (m_readbuffer != null) Array.Copy( m_readbuffer, new_readbuffer, readbuffer_size);
            Array.Copy( ar_, offset_, new_readbuffer, readbuffer_size, size_);
            m_readbuffer = new_readbuffer;
        }

        private int ConsumeReadbuffer(byte[] msg_, int offset_, int size_)
        {
            if (m_readbuffer != null)
            {
                int bufsize = m_readbuffer.Length;
                if (bufsize > size_)
                {
                    Array.Copy(m_readbuffer, msg_, size_);
                    byte[] new_readbuffer = new byte[bufsize - size_];
                    Array.Copy(m_readbuffer, size_, new_readbuffer, 0, bufsize - size_);
                    m_readbuffer = new_readbuffer;
                    return size_;
                }
                else
                {
                    Array.Copy(m_readbuffer, msg_, bufsize);
                    m_readbuffer = null;
                    return bufsize;
                }
            }
            return 0;
        }

        private int Read(byte[] msg_, int offset_, int size_)
        {
            int rt = ConsumeReadbuffer(msg_, offset_, size_);
            if (rt != 0) return rt;

            if (m_sslstream != null)
            {
                if (m_sslstream.CanRead)
                {
                    return m_sslstream.Read(msg_, offset_, size_);
                }
            }
            else
            {
                if (m_stream.CanRead)
                {
                    return m_stream.Read(msg_, offset_, size_);
                }
            }
            return 0;
        }

        private void CloseStream()
        {
            if (m_sslstream != null)
            {
                m_sslstream.Close();
            }
            else if (m_stream != null)
            {
                m_stream.Close();
            }
        }

        public static bool ValidateServerCertificate(
              object sender,
              X509Certificate certificate,
              X509Chain chain,
              SslPolicyErrors sslPolicyErrors)
        {
            // DEBUG: Console.WriteLine("Validate certificate");
            if (sslPolicyErrors == SslPolicyErrors.None)
                return true;

            // DEBUG: Console.WriteLine("Certificate error: {0}", sslPolicyErrors);
            return false;
        }

        public static bool AcceptServerCertificate(
              object sender,
              X509Certificate certificate,
              X509Chain chain,
              SslPolicyErrors sslPolicyErrors)
        {
            //... accept any certificate
            return true;
        }

        private void OpenStream()
        {
            if (m_config.sslcert != null)
            {
                X509Certificate cert = new X509Certificate2(m_config.sslcert, m_config.password);
                X509CertificateCollection certColl = new X509CertificateCollection();
                certColl.Add(cert);

                m_stream = null;
                RemoteCertificateValidationCallback validateCallback;
                if (m_config.validatecert)
                {
                    validateCallback = new RemoteCertificateValidationCallback(ValidateServerCertificate);
                }
                else
                {
                    validateCallback = new RemoteCertificateValidationCallback(AcceptServerCertificate);
                }
                m_sslstream = new SslStream(m_client.GetStream(), false, validateCallback, null);
                m_sslstream.AuthenticateAsClient(m_config.host, certColl, SslProtocols.Tls, false);
            }
            else
            {
                m_sslstream = null;
                m_stream = m_client.GetStream();
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
            m_readbuffer = null;
            m_lasterror = null;
        }

        public string lasterror()
        {
            return m_lasterror;
        }

        public bool Connect()
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
                    m_lasterror = "Could not resolve address";
                }
                else
                {
                    m_lasterror = err;
                }
                return false;
            }
            OpenStream();
            return true;
        }

        public void Close()
        {
            CloseStream();
            m_client.Close();
        }

        public byte[] ReadLine()
        {
            int bufsize = 64;
            byte[] ar = new byte[ bufsize];
            int offset = 0;
            while (true)
            {
                int chunksize = Read(ar, offset, bufsize - offset);
                int totsize = offset + chunksize;
                if (chunksize == 0)
                {
                    m_readbuffer = ar;
                    Array.Resize( ref m_readbuffer, totsize);
                    return null;
                }
                else
                {
                    for (; offset < totsize; offset++)
                    {
                        if (ar[ offset] == (byte)'\n')
                        {
                            if (offset +1 <= totsize)
                            {
                                CopyToReadBuffer( ar, offset+1, totsize - offset-1);
                            }
                            if (offset > 0 && ar[offset-1] == '\r')
                            {
                                offset--;
                            }
                            Array.Resize( ref ar, offset); 
                            return ar;
                        }
                    }
                    byte[] new_ar = new byte[ bufsize * 2];
                    Array.Copy( ar, new_ar, bufsize);
                    bufsize *= 2;
                    ar = new_ar;
                }
            }
        }

        public byte[] ReadContent()
        {
            int bufsize = 64;
            byte[] ar = new byte[ bufsize];
            int offset = 0;
            bool needMore = false;
            while (true)
            {
                needMore = false;
                int chunksize = Read(ar, offset, bufsize - offset);
                int totsize = offset + chunksize;
                if (chunksize == 0)
                {
                    m_readbuffer = ar;
                    Array.Resize( ref m_readbuffer, totsize);
                    return null;
                }
                else
                {
                    for (; offset < totsize && !needMore; offset++)
                    {
                        if (ar[ offset] == (byte)'\n')
                        {
                            int nextidx;
                            DetectEoD eod = doDetectEoD( ref ar, offset, totsize, out nextidx);
                            switch (eod)
                            {
                                case DetectEoD.NeedMoreData:
                                    offset -= 1;    //... compensate loop increment, we try again
                                    needMore = true;
                                    break;
                                case DetectEoD.No:
                                    continue;
                                case DetectEoD.Yes:
                                    if (nextidx < totsize)
                                    {
                                        CopyToReadBuffer( ar, nextidx, totsize - nextidx);
                                    }
                                    if (offset > 0 && ar[offset-1] == '\r')
                                    {
                                        offset--;
                                    }
                                    Array.Resize( ref ar, offset);
                                    return Protocol.UnescapeLFdot( ar);
                            }
                        }
                    }
                    byte[] new_ar = new byte[ bufsize * 2];
                    Array.Copy( ar, new_ar, bufsize);
                    bufsize *= 2;
                    ar = new_ar;
                }
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
            Write(msg_with_EoLn, 0, msg_with_EoLn.Length);
        }

        public void WriteContent( byte[] content)
        {
            UTF8Encoding utf8 = new UTF8Encoding(false/*no BOM*/, true/*throw if input illegal*/);
            byte[] content_encoded = Protocol.EscapeLFdot( content);
            int msglen = content_encoded.Length + 5/*EoD marker*/;
            byte[] msg = new byte[msglen];
            Array.Copy(content_encoded, 0, msg, 0, content_encoded.Length);
            int EoDidx = content_encoded.Length;
            msg[EoDidx + 0] = (byte)'\r';
            msg[EoDidx + 1] = (byte)'\n';
            msg[EoDidx + 2] = (byte)'.';
            msg[EoDidx + 3] = (byte)'\r';
            msg[EoDidx + 4] = (byte)'\n';
            Write(msg, 0, msg.Length);
        }
    };
}
