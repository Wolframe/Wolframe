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
            public string command { get; set; }
            public string doctype { get; set; }
            public string root { get; set; }
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
        private byte[] m_answerbuf;

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
            m_answerbuf = null;
        }

        private void HandleNextRequest()
        {
            try
            {
                m_request = m_requestqueue.Dequeue();
                if (m_request.command != null)
                {
                    m_connection.WriteLine("REQUEST " + m_request.command);
                }
                else
                {
                    m_connection.WriteLine("REQUEST");
                }
                byte[] requestcontent = Serializer.getRequestContent(m_request.doctype, m_request.root, m_request.answertype, m_request.content);
                m_connection.WriteContent(requestcontent);
                m_answerbuf = null;
                m_state = State.WaitAnswer;
            }
            catch (InvalidOperationException)
            {
                //... no more requests in the queue
                m_state = State.Idle;
                m_request = null;
                m_answerbuf = null;
            }
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

        public void HandleUnrecoverableError(string errstr)
        {
            if (m_request != null)
            {
                m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = errstr });
                m_request = null;
            }
            m_processError(errstr);
            ClearRequestQueue(errstr);
            m_connection.Close();
            m_state = State.Terminated;
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
            HandleUnrecoverableError(errstr);
        }

        public bool IsCommand(string cmd,byte[] msg)
        {
            if (cmd.Length > msg.Length) return false;
            if (msg.Length > cmd.Length && msg[cmd.Length] != (byte)' ') return false;
            int ii = 0;
            for (; ii<cmd.Length; ++ii) if (msg[ii] != (byte)cmd[ii]) return false;
            return true;
        }

        public void ProcessConnectionMessageDelegate(byte[] msg)
        {
            switch (m_state)
            {
                case State.Init:
                    HandleUnrecoverableError("protocol error in init");
                    break;

                case State.Connecting:
                    //... get empty message here when connection succeded
                    m_state = State.HandshakeReadBanner;
                    m_connection.IssueReadLine();
                    break;

                case State.HandshakeReadBanner:
                    m_banner = Encoding.UTF8.GetString(msg);
                    m_state = State.HandshakeGrantConnect;
                    m_connection.IssueReadLine();
                    break;

                case State.HandshakeGrantConnect:
                    if (IsCommand("OK", msg))
                    {
                        m_connection.WriteLine("AUTH");
                        m_state = State.HandshakeMechs;
                        m_connection.IssueReadLine();
                    }
                    else
                    {
                        HandleUnrecoverableError("connection refused");
                    }
                    break;

                case State.HandshakeMechs:
                    if (IsCommand("MECHS", msg))
                    {
                        m_connection.WriteLine("MECH NONE");
                        m_state = State.HandshakeGrantMechNONE;
                        m_connection.IssueReadLine();
                    }
                    else
                    {
                        HandleUnrecoverableError("authorization process refused");
                    }
                    break;

                case State.HandshakeGrantMechNONE:
                    if (IsCommand("OK", msg))
                    {
                        m_state = State.Idle; //... authorized now
                        HandleNextRequest();
                    }
                    else
                    {
                        HandleUnrecoverableError("authorization refused");
                    }
                    break;

                case State.Idle:
                    HandleUnrecoverableError("got unexpected message in idle state");
                    break;

                case State.WaitAnswer:
                    if (IsCommand("ANSWER", msg))
                    {
                        m_state = State.WaitAnswerResult;
                        m_connection.IssueReadContent();
                    }
                    else if (IsCommand("OK", msg))
                    {
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Result, id = m_request.id, content = null });
                        HandleNextRequest();
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
                        HandleNextRequest();
                    }
                    else
                    {
                        HandleUnrecoverableError("protocol error in wait answer");
                    }
                    break;

                case State.WaitAnswerResult:
                    m_answerbuf = msg;
                    m_state = State.WaitAnswerSuccess;
                    m_connection.IssueReadLine();
                    break;

                case State.WaitAnswerSuccess:
                    if (IsCommand("OK", msg))
                    {
                        object result = Serializer.getResult(m_answerbuf, m_request.answertype);
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Result, id = m_request.id, content = result });
                        HandleNextRequest();
                    }
                    else if (IsCommand("ERR", msg))
                    {
                        string arg = Encoding.UTF8.GetString(msg, 3, msg.Length - 3);
                        m_processAnswer(new Answer { msgtype = Answer.MsgType.Failure, id = m_request.id, content = arg });
                        HandleNextRequest();
                    }
                    else
                    {
                        HandleUnrecoverableError("protocol error after answer result");
                    }
                    break;

                case State.WaitQuit:
                    if (IsCommand("BYE", msg))
                    {
                        m_connection.Close();
                        m_state = State.Terminated;
                    }
                    else
                    {
                        // ... Ignore message (already handled in .Close())
                    }
                    break;

                case State.Terminated:
                    HandleUnrecoverableError("got unexpected message after termination");
                    break;
            }
        }

        void HandleCloseInWaitAnswer()
        {
            m_processAnswer(new Answer { msgtype = Answer.MsgType.Error, id = m_request.id, content = msgstr });
            m_connection.WriteLine("QUIT");
            ClearRequestQueue("client closed connection");
            m_state = State.WaitQuit;
            m_connection.IssueReadLine();
        }

        void Close()
        {
            string msgstr = "client closed connection";
            switch (m_state)
            {
                case State.Init:
                    m_connection.Close();
                    break;
                case State.Connecting:
                    HandleUnrecoverableError("client closed connection");
                    break;
                case State.HandshakeReadBanner:
                    HandleUnrecoverableError("client closed connection");
                    break;
                case State.HandshakeGrantConnect:
                    HandleUnrecoverableError("client closed connection");
                    break;
                case State.HandshakeMechs:
                    HandleUnrecoverableError("client closed connection");
                    break;
                case State.HandshakeGrantMechNONE:
                    HandleUnrecoverableError("client closed connection");
                    break;
                case State.Idle:
                    m_connection.WriteLine("QUIT");
                    ClearRequestQueue(msgstr);
                    m_state = State.WaitQuit;
                    m_connection.IssueReadLine();
                    break;
                case State.WaitAnswer:
                    HandleCloseInWaitAnswer();
                    break;
                case State.WaitAnswerResult:
                    HandleCloseInWaitAnswer();
                    break;
                case State.WaitAnswerSuccess:
                    HandleCloseInWaitAnswer();
                    break;
                case State.WaitQuit:
                    m_connection.Close();
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
