using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Net.Sockets;
using System.Collections.Concurrent;
using WolframeClient;

namespace WolframeClient
{
    public class Session
        :SessionInterface
    {
        public class Configuration
            :Connection.Configuration
        {
            public string banner { get; set; }
            public string authmethod { get; set; }
            public string username { get; set; }
            public string password { get; set; }
            public string schemadir { get; set; }
            public string schemaext { get; set; }

            public Configuration()
            {
                banner = null;
                authmethod = null;
                username = null;
                password = null;
                schemadir = "http://www.wolframe.net";
                schemaext = "xsd";
            }
        };

        public delegate void AnswerCallback(Answer msg);

        private class PendingRequest
        {
            public int id { get; set; }
            public int number { get; set; }
            public Type answertype { get; set; }
        };
        private Configuration m_config;
        private Serializer m_serializer;
        private Connection m_connection;
        private volatile string m_lasterror;
        public enum State { Init, Running, Shutdown, Terminated };
        private volatile State m_state;
        private object m_stateLock;
        private WorkQueue<Request> m_requestqueue;
        private WorkQueue<PendingRequest> m_pendingqueue;
        private AnswerCallback m_answerCallback;
        private Thread m_request_thread;
        private Thread m_answer_thread;

        private void SetState(State state, string err)
        {
            lock (m_stateLock)
            {
                if (m_state != State.Terminated)
                {
                    if (m_state == State.Shutdown && state != State.Terminated)
                    {
                    }
                    else
                    {
                        m_state = state;
                    }
                }
                if (m_lasterror == null)
                {
                    m_lasterror = err;
                }
            }
        }

        private State GetState()
        {
            lock (m_stateLock)
            {
                return m_state;
            }
        }

        private bool AuthMechWolframeCram()
        {
		    // 1. The client sends a 256 bit seed followed by a HMAC-SHA1 of the (seed, username)
		    m_connection.WriteContent(
                Encoding.ASCII.GetBytes( AuthWolframeCram.UsernameHash( m_config.username))); 

		    // 2. If the server finds the user it will reply with a seed and a challenge. 
		    //	If the user is not found it will reply with a random challenge for not
		    //	giving any information to the client. The procedure will continue.
            byte[] challenge = m_connection.ReadContent();
            if (challenge == null)
            {
                SetState(State.Terminated, "server closed connection");
                return false;
            }
		    // 3. The client returns a response. The response is computed from the PBKDF2 
		    //	of the seed and the challenge.
            string response = AuthWolframeCram.CRAMresponse(m_config.password, Encoding.ASCII.GetString(challenge));
            m_connection.WriteContent( Encoding.ASCII.GetBytes( response));

		    // 4. the server tries to authenticate the user and returns "OK" in case
		    //	of success, "ERR" else.
            byte[] ln = m_connection.ReadLine();
            if (ln == null)
            {
                SetState(State.Terminated, "server closed connection");
                return false;
            }
            if (Protocol.IsCommand("OK", ln))
            {
                return true;
            }
            else if (Protocol.IsCommand("ERR", ln))
            {
                SetState(State.Terminated, "authentication failed: " + Protocol.CommandArg("ERR", ln));
                return false;
            }
            else
            {
                SetState(State.Terminated, "protocol error in authorization");
                return false;
            }
        }

        private void HandleAnswer(PendingRequest rq)
        {
            byte[] ln = m_connection.ReadLine();
            if (ln == null)
            {
                SetState(State.Terminated, "server closed connection");
                return;
            }
            else if (Protocol.IsCommand("OK", ln))
            {
                m_answerCallback(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, number = rq.number, obj = null });
            }
            else if (Protocol.IsCommand("ERR", ln))
            {
                m_answerCallback(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, number = rq.number, obj = Protocol.CommandArg("ERR", ln) });
            }
            else if (Protocol.IsCommand("BAD", ln))
            {
                SetState(State.Terminated, "protocol error");
                m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "protocol error" });
                return;
            }
            else if (Protocol.IsCommand("ANSWER", ln))
            {
                byte[] msg = m_connection.ReadContent();
                if (msg == null)
                {
                    SetState(State.Terminated, "server closed connection");
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "server closed connection" });
                    return;
                }
                ln = m_connection.ReadLine();
                if (msg == null)
                {
                    SetState(State.Terminated, "server closed connection");
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "server closed connection" });
                    return;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    object answerobj = m_serializer.getResult(msg, rq.answertype);
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, number = rq.number, obj = answerobj });
                }
                else if (Protocol.IsCommand("ERR", ln))
                {
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, number = rq.number, obj = Protocol.CommandArg("ERR", ln) });
                }
                else if (Protocol.IsCommand("BAD", ln))
                {
                    SetState(State.Terminated, "protocol error");
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "protocol error" });
                    return;
                }
            }
        }

        private void HandleRequest(Request rq)
        {
            byte[] rqdata = m_serializer.getRequestContent(rq.doctype, rq.root, rq.objtype, rq.obj);
            m_pendingqueue.Enqueue(new PendingRequest { id = rq.id, number = rq.number, answertype = rq.answertype });
            if (rq.command != null && rq.command.Length > 0)
            {
                m_connection.WriteLine("REQUEST " + rq.command);
            }
            else
            {
                m_connection.WriteLine("REQUEST");
            }
            m_connection.WriteContent( rqdata);
        }

        private void RunRequests()
        {
            // This method is called by Connect in an own thread and processes the write of requests to the server
            Request rq = null;
            while (m_requestqueue.Dequeue(out rq))
            {
                HandleRequest(rq);
            }
            ClearRequestQueue();
        }

        private void RunAnswers()
        {
            // This method is called by Connect in an own thread and processes the reading of answers from the server
            PendingRequest rq = null;
            while (m_pendingqueue.Dequeue(out rq))
            {
                HandleAnswer(rq);
            }
            if (GetState() == State.Shutdown)
            {
                ProcessPendingRequests();
            }
        }

        private void ProcessPendingRequests()
        {
            PendingRequest rq = null;
            while (m_pendingqueue.DequeueUnblocking(out rq))
            {
                HandleAnswer(rq);
            }
        }

        private void ClearRequestQueue()
        {
            Request rq = null;
            while (m_requestqueue.DequeueUnblocking(out rq))
            {
                if (m_lasterror != null)
                {
                    string msg = "session terminated: " + m_lasterror;
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = msg });
                }
                else
                {
                    string msg = "session terminated";
                    m_answerCallback(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = msg });
                }
            }
        }

/* PUBLIC METHODS: */
        public Session( Configuration config_, AnswerCallback answerCallback_)
        {
            m_config = config_;
            m_serializer = new Serializer(m_config.schemadir, m_config.schemaext);
            m_connection = new Connection( m_config);

            m_lasterror = null;
            m_state = State.Init;
            m_stateLock = new object();

            m_requestqueue = new WorkQueue<Request>('R');
            m_pendingqueue = new WorkQueue<PendingRequest>('A');

            m_answerCallback = answerCallback_;
            m_request_thread = null;
            m_answer_thread = null;
        }

        public void Shutdown()
        {
            if (GetState() == State.Running)
            {
                SetState( State.Shutdown, null);
                m_requestqueue.Terminate();
                m_pendingqueue.Terminate();
            }
        }

        public void Close()
        {
            Shutdown();
            if (GetState() == State.Shutdown)
            {
                m_connection.Close();
                m_request_thread.Join();
                m_answer_thread.Join();
            }
        }

        public bool Connect()
        {
            try
            {
                if (!m_connection.Connect())
                {
                    SetState(State.Terminated, m_connection.lasterror());
                }
                byte[] ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    SetState(State.Terminated, "server closed connection");
                    return false;
                }
                string banner = Encoding.UTF8.GetString(ln);
                if (m_config.banner != null && m_config.banner != banner)
                {
                    SetState(State.Terminated, "unknown service connected (banner does not match).");
                    return false;
                }
                ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    SetState(State.Terminated, "server closed connection");
                    return false;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    m_connection.WriteLine("AUTH");
                    ln = m_connection.ReadLine();
                    if (ln == null) 
                    {
                        SetState(State.Terminated, "server closed connection");
                        return false;
                    }
                    else if (Protocol.IsCommand("MECHS", ln))
                    {
                        if (m_config.authmethod != null)
                        {
                            m_connection.WriteLine("MECH " + m_config.authmethod);
                        }
                        else
                        {
                            m_connection.WriteLine("MECH NONE");
                        }
                        ln = m_connection.ReadLine();
                        if (ln == null) 
                        {
                            SetState(State.Terminated, "server closed connection");
                            return false;
                        }
                        else if (Protocol.IsCommand("OK", ln))
                        {
                            if (m_config.authmethod == null || Protocol.IsEqual(m_config.authmethod, "NONE"))
                            {
                                //... no authentication
                            }
                            else if (Protocol.IsEqual(m_config.authmethod, "WOLFRAME-CRAM"))
                            {
                                if (!AuthMechWolframeCram())
                                {
                                    return false;
                                }
                            }
                            m_request_thread = new Thread(new ThreadStart(this.RunRequests));
                            m_answer_thread = new Thread(new ThreadStart(this.RunAnswers));
                            SetState(State.Running, null);
                            m_request_thread.Start();
                            m_answer_thread.Start();
                            return true;
                        }
                        else if (Protocol.IsCommand("ERR", ln))
                        {
                            SetState(State.Terminated, "authorization denied: " + Protocol.CommandArg( "ERR", ln));
                            return false;
                        }
                        else
                        {
                            SetState(State.Terminated, "authorization denied");
                            return false;
                        }
                    }
                    else if (Protocol.IsCommand("ERR", ln))
                    {
                        m_lasterror = Protocol.CommandArg( "ERR", ln);
                        return false;
                    }
                    else
                    {
                        SetState(State.Terminated, "authorization process refused");
                        return false;
                    }
                }
                SetState(State.Terminated, "protocol error");
                return false;
            }
            catch (Exception soe)
            {
                SetState(State.Terminated, soe.Message);
                return false;
            }
        }

        public void IssueRequest( Request request)
        {
            m_requestqueue.Enqueue( request);
        }

        public int NofOpenRequests()
        {
            return m_requestqueue.Count() + m_pendingqueue.Count();
        }

        public string GetLastError()
        {
            lock (m_stateLock)
            {
                string err = m_lasterror;
                m_lasterror = null;
                return err;
            }
        }
    };
}
