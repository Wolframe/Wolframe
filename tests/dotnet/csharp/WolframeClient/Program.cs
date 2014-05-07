using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Serialization;
using System.Text.RegularExpressions;
using System.Net;
using System.Net.Sockets;
using System.Collections;

public class Link
{
    [XmlElement("originalUrl")]
    public string OriginalUrl { get; set; }
    // You other props here much like the above
}

[XmlRoot("profileSite")]
public class ProfileSite
{
    [XmlElement("profileId")]
    public int ProfileId { get; set; }

    [XmlElement("siteId")]
    public int SiteId { get; set; }

    [XmlArray("links"), XmlArrayItem("link")]
    public Link[] Links { get; set; }

}

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
        public enum State { Init = 0, Connecting, Connected };
        private State m_state;
        private byte[] m_buffer;

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
            m_buffer = null;
            m_processError = processError;
            m_processMessage = processMessage;
            m_state = State.Init;
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
            }
            catch
            {
                m_error = Error.WriteFailed;
                m_processError( m_error);
            }
        }

        public void pushReceivedBuffer( byte[] msgbuf)
        {
            if (m_buffer != null)
            {
                byte[] joinbuf = new byte[ m_buffer.Length + msgbuf.Length];
                Array.Copy( m_buffer, 0, joinbuf, 0, m_buffer.Length);
                Array.Copy( msgbuf, 0, joinbuf, m_buffer.Length, msgbuf.Length);
                m_buffer = joinbuf;
            }
            else
            {
                m_buffer = msgbuf;
            }
        }

        public byte[] escapeLFdot( byte[] msg)
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
            rt = new byte[ size];

            while (0 < (nextidx = Array.IndexOf(msg, '\n', idx)))
            {
                Array.Copy(msg, idx, rt, rtidx, nextidx - idx + 1);
                rtidx = rtidx + nextidx - idx + 1;
                idx = nextidx + 1;

                if (msg.Length >= idx)
                {
                    if (msg[idx] == '.')
                    {
                        rt[ rtidx] = (byte)'.';
                        rtidx = rtidx + 1;
                    }
                }
            }
            Array.Copy(msg, idx, rt, rtidx, msg.Length - idx);
            return rt;
        }

        public byte[] unescapeLFdot( byte[] msg)
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

        public byte[] getMessageFromBuffer( int idx, int nextidx)
        {
            byte[] msg = new byte[idx];
            Array.Copy( m_buffer, 0, msg, 0, idx);
            if (m_buffer.Length > nextidx)
            {
                byte[] rest_buffer = new byte[ m_buffer.Length - nextidx];
                Array.Copy( m_buffer, nextidx, rest_buffer, 0, rest_buffer.Length);
                m_buffer = rest_buffer;
            }
            else
            {
                m_buffer = null;
            }
            return msg;
        }

        public byte[] fetchContentFromBuffer()
        {
            int idx = -1;
            int nextidx = 0;

            // Find LF . CRLF or LF . LF marking the end of message:
            while (nextidx == 0 && 0 < (idx = Array.IndexOf( m_buffer, '\n', idx+1)))
            {
                if (m_buffer.Length >= idx + 2)
                {
                    if (m_buffer[idx + 1] == '.')
                    {
                        if (m_buffer[idx + 2] == '\n')
                        {
                            nextidx = idx + 3;
                        }
                        else if (m_buffer[idx + 2] == '\r')
                        {
                            if (m_buffer.Length >= idx + 3 && m_buffer[idx + 3] == '\n')
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
                return unescapeLFdot( getMessageFromBuffer( idx, nextidx));
            }
            return null;
        }

        public byte[] fetchLineFromBuffer()
        {
            int idx = Array.IndexOf( m_buffer, '\n');
            if (idx < 0) return null;
            if (idx > 0 && m_buffer[idx - 1] == '\r')
            {
                return getMessageFromBuffer( idx - 1, idx + 1);
            }
            else
            {
                return getMessageFromBuffer( idx, idx + 1);
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

        public void WriteBytes( byte[] bytes)
        {
            m_stream.BeginWrite( bytes, 0, bytes.Length, WriteCallback, null);
        }

        public void WriteLn( string msgstr)
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
            byte[] content = escapeLFdot(bytes);
            byte[] content_with_EoD = new byte[ content.Length + 5];
            content_with_EoD[ content.Length + 0] = (byte)'\r';
            content_with_EoD[ content.Length + 1] = (byte)'\n';
            content_with_EoD[ content.Length + 2] = (byte)'.';
            content_with_EoD[ content.Length + 3] = (byte)'\r';
            content_with_EoD[ content.Length + 4] = (byte)'\n';
            WriteBytes( content_with_EoD);
        }
    };

    class Session
    {
        private string m_banner;
        private Connection m_connection;
        private string m_authmethod;
        private enum State { Init, Connecting, Handshake, Idle, WaitAnswer, WaitQuit };
        private State m_state;

        public class Request
        {
            public int id { get; set; }
            public object content { get; set; }
            public Type answertype { get; set; }
        };

        public class Answer
        {
            public enum MsgType { Error, Result };
            public MsgType msgtype { get; set; }
            public int id { get; set; }
            public object content { get; set; }
        };

        public delegate void ProcessSessionErrorDelegate( string err);
        public delegate void ProcessAnswerDelegate( Answer obj);

        private ProcessSessionErrorDelegate m_processError;
        private ProcessAnswerDelegate m_processAnswer;

        private Queue<Request> m_requestqueue;
        private Request m_request;

        private void ClearRequestQueue( string errstr)
        {
            while (m_requestqueue.Count() > 0)
            {
                Request request = m_requestqueue.Dequeue();
                Answer answer = new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr };
                m_processAnswer(answer);
            }
        }

        public void ProcessConnectionErrorDelegate( Connection.Error err)
        {
            string errstr = null;
            switch (err)
            {
                case Connection.Error.NoError: m_processError( "unknown error"); return;
                case Connection.Error.ConnectionFailed: m_processError( "connection failed"); return;
                case Connection.Error.WriteFailed: errstr = "write failed"; break;
                case Connection.Error.ReadFailed: errstr = "read failed"; break;
                case Connection.Error.ConnectionClosed: errstr = "server closed connection"; break;
            }
            switch (m_state)
            {
                case State.Init: m_processError(errstr); break;
                case State.Connecting: m_processError(errstr); break;
                case State.Handshake: m_processError(errstr); break;
                case State.Idle: m_processError(errstr); break;
                case State.WaitAnswer:
                {
                    Answer answer = new Answer{msgtype=Answer.MsgType.Error, id=m_request.id, content=errstr};
                    m_processAnswer( answer);
                    break;
                }
            }
        }

        public void ProcessConnectionMessageDelegate(byte[] msg)
        {

        }

        Session( string ip, int port, string authmethod, ProcessSessionErrorDelegate processError_, ProcessAnswerDelegate processAnswer_)
        {
            m_banner = null;
            m_connection = new Connection( ip, port, ProcessConnectionErrorDelegate, ProcessConnectionMessageDelegate);
            m_authmethod = authmethod;
            m_state = State.Init;

            m_processError = processError_;
            m_processAnswer = processAnswer_;

            m_requestqueue = new Queue<Request>();
            m_request = null;
        }

        void Close()
        {
            string msgstr = "client closed connection";
            switch (m_state)
            {
                case State.WaitAnswer:
                {
                    Answer answer = new Answer{msgtype=Answer.MsgType.Error, id=m_request.id, content=msgstr};
                    m_processAnswer( answer);
                    ClearRequestQueue( msgstr);
                    break;
                }
                /*no break here!*/
                case State.Init:
                case State.Connecting:
                case State.Handshake:
                    m_processError(msgstr);
                    break;
                case State.Idle:
                    m_connection.WriteLn("QUIT");
                    ClearRequestQueue(msgstr);
                    break;
            }
        }

        void Connect()
        {
            if (m_state == State.Init)
            {
                m_connection.Connect();
            }
        }
    }

    class Program
    {
        static string getRequestString( string doctype, string root, Type type, object obj)
        {
            var xattribs = new XmlAttributes();
            var xroot = new XmlRootAttribute( root);
            xattribs.XmlRoot = xroot;
            var xoverrides = new XmlAttributeOverrides();
            //... have to use XmlAttributeOverrides because .NET insists on the object name as root element name otherwise ([XmlRoot(..)] has no effect)
            xoverrides.Add( type, xattribs);

            XmlSerializer serializer = new XmlSerializer( type, xoverrides);
            StringWriter sw = new StringWriter();
            XmlWriterSettings wsettings = new XmlWriterSettings();
            wsettings.OmitXmlDeclaration = false;
            wsettings.Encoding = new UTF8Encoding();
            XmlWriter xw = XmlWriter.Create( sw, wsettings);
            xw.WriteProcessingInstruction( "xml", "version='1.0' standalone='no'");
            //... have to write header by hand (OmitXmlDeclaration=false has no effect)
            xw.WriteDocType( root, null, doctype + ".sfrm", null);

            XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
            ns.Add("", "");
            //... trick to avoid printing of xmlns:xsi xmlns:xsd attributes of the root element

            serializer.Serialize( xw, obj, ns);
            return sw.ToString();
        }

        static object getAnswer( string content, Type type)
        {
            Regex doctypeDeclaration = new Regex("<!DOCTYPE[ ]*[^>]*[>]");
             string saveContent = doctypeDeclaration.Replace( content, "");
             // ... !DOCTYPE is prohibited for security reasons by .NET, if not 
             //      explicitely enabled. We cut it out, because we do not need it.
             StringReader sr = new StringReader( saveContent);
             XmlReader xr = XmlReader.Create( sr);
             XmlSerializer xs = new XmlSerializer( type);
             return xs.Deserialize( xr); 
        }

        static void Main(string[] args)
        {
            ProfileSite obj = new ProfileSite();
            obj.ProfileId = 1;
            obj.SiteId = 2;
            Link[] lnk1 = new Link[2];
            lnk1[0] = new Link();
            lnk1[1] = new Link();
            lnk1[0].OriginalUrl = "wolframe.net";
            lnk1[1].OriginalUrl = "test.net";
            obj.Links = lnk1;

            var res = getRequestString("profileSite", "profileSite", typeof(ProfileSite), obj);
            Console.WriteLine("\nDocument:\n{0}", res);
            ProfileSite inv = (ProfileSite)getAnswer(res, typeof(ProfileSite));
            Console.WriteLine("Object ProfileId: {0}", inv.ProfileId);
            Console.WriteLine("Object SiteId: {0}", inv.SiteId);
            foreach (Link lnk in inv.Links)
            {
                Console.WriteLine("Object Link: {0}", lnk.OriginalUrl);
            }

            TcpClient client = new TcpClient( "127.0.0.1", 7661);
            NetworkStream netStream = client.GetStream();
        }
    }
}
