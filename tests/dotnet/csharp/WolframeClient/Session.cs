using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using WolframeClient;

namespace WolframeClient
{
    class Session
    {
        private string m_banner;
        private Connection m_connection;
        private string m_authmethod;
        private enum State
        {
            Init, 
            Connecting,
            HandshakeReadBanner,
            HandshakeGrantConnect,
            HandshakeMechs,
            HandshakeGrantMechNONE,
            Idle,
            WaitAnswer,
            WaitAnswerResult,
            WaitAnswerSuccess,
            WaitQuit,
            Terminated
        };
        private State m_state;

        public class Request
        {
            public int id { get; set; }
            public object content { get; set; }
            public Type answertype { get; set; }
        };

        public class Answer
        {
            public enum MsgType { Error, Failure, Result };
            public MsgType msgtype { get; set; }
            public int id { get; set; }
            public object content { get; set; }
        };

        public delegate void ProcessSessionErrorDelegate(string err);
        public delegate void ProcessAnswerDelegate(Answer obj);

        private ProcessSessionErrorDelegate m_processError;
        private ProcessAnswerDelegate m_processAnswer;

        private Queue<Request> m_requestqueue;
        private Request m_request;

        Session(string ip, int port, string authmethod, ProcessSessionErrorDelegate processError_, ProcessAnswerDelegate processAnswer_)
        {
            m_banner = null;
            m_connection = new Connection(ip, port, ProcessConnectionErrorDelegate, ProcessConnectionMessageDelegate);
            m_authmethod = authmethod;
            m_state = State.Init;

            m_processError = processError_;
            m_processAnswer = processAnswer_;

            m_requestqueue = new Queue<Request>();
            m_request = null;
        }

        private void ClearRequestQueue(string errstr)
        {
            bool empty = false;
            while (!empty)
            {
                try
                {
                    Request request = m_requestqueue.Dequeue();
                    Answer answer = new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr };
                    m_processAnswer(answer);
                }
                catch (InvalidOperationException)
                {
                    //... done
                    empty = true;
                }
            }
        }

        public void ProcessConnectionErrorDelegate(Connection.Error err)
        {
            string errstr = null;
            switch (err)
            {
                case Connection.Error.NoError: m_processError("unknown error"); return;
                case Connection.Error.ConnectionFailed: m_processError("connection failed"); return;
                case Connection.Error.WriteFailed: errstr = "write failed"; break;
                case Connection.Error.ReadFailed: errstr = "read failed"; break;
                case Connection.Error.ConnectionClosed: errstr = "server closed connection"; break;
            }
            switch (m_state)
            {
                case State.Init:
                    m_processError(errstr);
                    break;
                case State.Connecting:
                    m_processError(errstr);
                    break;
                case State.HandshakeReadBanner:
                    m_processError(errstr);
                    break;
                case State.HandshakeGrantConnect:
                    m_processError(errstr);
                    break;
                case State.HandshakeMechs:
                    m_processError(errstr);
                    break;
                case State.HandshakeGrantMechNONE:
                    m_processError(errstr);
                    break;
                case State.Idle:
                    m_processError(errstr);
                    ClearRequestQueue(errstr);
                    break;
                case State.WaitAnswer:
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr });
                    ClearRequestQueue(errstr);
                    break;
                case State.WaitAnswerResult:
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr });
                    ClearRequestQueue(errstr);
                    break;
                case State.WaitAnswerSuccess:
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr });
                    ClearRequestQueue(errstr);
                    break;
                case State.WaitQuit:
                    m_processError(errstr + " after QUIT");
                    break;
                case State.Terminated:
                    m_processError(errstr + " after termination");
                    break;
            }
            m_request = null;
            m_connection.Close();
            m_state = State.Terminated;
        }

        public bool IsCommand(string cmd,byte[] msg)
        {
            if (cmd.Length > msg.Length) return false;
            if (msg.Length > cmd.Length && msg[cmd.Length] != (byte)' ') return false;
            int ii = 0;
            for (; ii<cmd.Length; ++ii) if (msg[ii] != (byte)cmd[ii]) return false;
            return true;
        }

        public string GetProtocolLine(string cmd, byte[] msg)
	    {
            if (IsCommand(cmd,msg))
            {
                string arg = Encoding.UTF8.GetString( msg, cmd.Length, msg.Length - cmd.Length);
                return arg;
            }
            if (IsCommand("ERR",msg))
            {
                string arg = Encoding.UTF8.GetString(msg, cmd.Length, msg.Length - cmd.Length);
                if (m_state == State.WaitAnswer)
                {
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Failure, id = m_request.id, content = arg });
                }
                else
                {
                    m_processError(arg);
                }
                return null;
            }
            else if (IsCommand("BAD",msg))
            {
                string arg = Encoding.UTF8.GetString(msg, 3, msg.Length - 3);
                if (m_state == State.WaitAnswer)
                {
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = arg });
                }
                else
                {
                    m_processError(arg);
                }
                m_connection.WriteLine("QUIT");
                ClearRequestQueue(arg);
                m_state = State.WaitQuit;
            }
            else if (IsCommand("BYE", msg))
            {
                string arg = Encoding.UTF8.GetString(msg, 3, msg.Length - 3);
                if (m_state == State.WaitAnswer)
                {
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = arg });
                }
                else
                {
                    m_processError("connection closed (" + arg + ")");
                }
                ClearRequestQueue("connection closed (" + arg + ")");
                m_connection.Close();
                m_state = State.Terminated;
            }
	    }

        public void ProcessConnectionMessageDelegate(byte[] msg)
        {
            switch (m_state)
            {
                case State.WaitQuit:
                    if (IsCommand("BYE", msg))
                    {
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    else
                    {
                        m_processError("protocol error after QUIT");
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    break;

                case State.Init:
                    m_processError("protocol error in init");
                    m_connection.Close();
                    break;

                case State.Connecting:
                    //... get empty message here when connection succeded
                    m_state = State.HandshakeReadBanner;
                    m_connection.IssueReadLine();
                    break;

                case State.HandshakeReadBanner:
                    m_banner = Encoding.UTF8.GetString(msg);
                    m_connection.IssueReadLine();
                    m_state = State.HandshakeGrantConnect;
                    break;

                case State.HandshakeGrantConnect:
                    if (IsCommand("OK", msg))
                    {
                        m_connection.WriteLine("AUTH");
                        m_connection.IssueReadLine();
                        m_state = State.HandshakeMechs;
                    }
                    else
                    {
                        m_processError("connection refused");
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    break;

                case State.HandshakeMechs:
                    if (IsCommand("MECHS", msg))
                    {
                        m_connection.WriteLine("MECH NONE");
                        m_connection.IssueReadLine();
                        m_state = State.HandshakeGrantMechNONE;
                    }
                    else
                    {
                        m_processError("authorization process refused");
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    break;

                case State.HandshakeGrantMechNONE:
                    if (IsCommand("OK", msg))
                    {
                        m_state = State.Idle;
                    }
                    else
                    {
                        m_processError("authorization refused");
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    break;

                case State.Idle:
                    m_connection.WriteLine("QUIT");
                    ClearRequestQueue("unexpected ");
                    m_state = State.WaitQuit;
                    break;

                case State.WaitAnswer:
                    if (IsCommand("OK", msg))
                    {
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Result, id = m_request.id, content = null });
                    }
                    else if (IsCommand("BAD", msg))
                    {
                        string arg = Encoding.UTF8.GetString(msg, 3, msg.Length - 3);
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = arg });
                        ClearRequestQueue(arg);
                        m_connection.WriteLine("QUIT");
                        m_state = State.WaitQuit;
                        //... we do not try to recover from protocol errors
                    }
                    else if (IsCommand("ERR", msg))
                    {
                        string arg = Encoding.UTF8.GetString(msg, 3, msg.Length - 3);
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Failure, id = m_request.id, content = arg });
                        m_state = State.Idle;
                    }
                    break;
                case State.WaitAnswer:
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Result, id = m_request.id, content = errstr });
                    ClearRequestQueue(errstr);
                    break;
                case State.WaitQuit:
                    m_processError(errstr + " after QUIT");
                    break;
                case State.Terminated:
                    m_processError(errstr + " after termination");
                    break;
            }
        }

        void Close()
        {
            string msgstr = "client closed connection";
            switch (m_state)
            {
                case State.WaitQuit:
                    m_connection.Close();
                    break;
                case State.WaitAnswer:
                    m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = msgstr });
                    m_connection.WriteLine("QUIT");
                    ClearRequestQueue(msgstr);
                    m_state = State.WaitQuit;
                    break;
                case State.Init:
                    m_connection.Close();
                    break;
                case State.Connecting:
                    m_connection.Close();
                    ClearRequestQueue(msgstr);
                    m_state = State.Terminated;
                    break;
                case State.Handshake:
                    m_connection.Close();
                    ClearRequestQueue(msgstr);
                    m_state = State.Terminated;
                    break;
                case State.Idle:
                    m_connection.WriteLine("QUIT");
                    ClearRequestQueue(msgstr);
                    m_state = State.WaitQuit;
                    break;
                case State.Terminated:
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
    };
}
