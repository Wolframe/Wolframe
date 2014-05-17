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
        public delegate void AnswerCallback(Answer msg);

        private class PendingRequest
        {
            public int id { get; set; }
            public int number { get; set; }
            public Type answertype { get; set; }
        };

        private string m_banner;
        private Connection m_connection;
        private string m_authmethod;
        private volatile string m_lasterror;
        public enum State { Init, Running, Shutdown, Terminated };
        private volatile State m_state;
        private object m_stateLock;
        private AutoResetEvent m_requestqueue_signal;
        private ConcurrentQueue<Request> m_requestqueue;
        private AutoResetEvent m_pendingqueue_signal;
        private ConcurrentQueue<PendingRequest> m_pendingqueue;
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
                    object answerobj = Serializer.getResult(msg, rq.answertype);
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
            byte[] rqdata = Serializer.getRequestContent(rq.doctype, rq.root, rq.objtype, rq.obj);
            m_pendingqueue.Enqueue(new PendingRequest { id = rq.id, number = rq.number, answertype = rq.answertype });
            m_connection.WriteRequest(rq.command, rqdata);
            m_pendingqueue_signal.Set();
        }

        private void RunRequests()
        {
            // This method is called by Connect in an own thread and processes the write of requests to the server
            while (GetState() == State.Running)
            {
                Request rq = null;
                if (m_requestqueue.TryDequeue(out rq))
                {
                    HandleRequest( rq);
                }
                else
                {
                    m_requestqueue_signal.WaitOne();
                }
            }
            ClearRequestQueue();
        }

        private void RunAnswers()
        {
            // This method is called by Connect in an own thread and processes the reading of answers from the server
            SetState( State.Running, null);
            while (GetState() == State.Running)
            {
                PendingRequest rq = null;
                if (m_pendingqueue.TryDequeue( out rq))
                {
                    HandleAnswer( rq);
                }
                else
                {
                    m_pendingqueue_signal.WaitOne();
                }
            }
            if (GetState() == State.Shutdown)
            {
                ProcessPendingRequests();
            }
        }

        private void ProcessPendingRequests()
        {
            bool empty = false;
            while (!empty && GetState() != State.Terminated)
            {
                PendingRequest rq = null;
                if (m_pendingqueue.TryDequeue(out rq))
                {
                    HandleAnswer(rq);
                }
                else
                {
                    //... done
                    empty = true;
                }
            }
        }

        private void ClearRequestQueue()
        {
            bool empty = false;
            while (!empty)
            {
                Request rq = null;
                if (m_requestqueue.TryDequeue(out rq))
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
                else
                {
                    //... done
                    empty = true;
                }
            }
        }

/* PUBLIC METHODS: */
        public Session(string ip, int port, string authmethod, AnswerCallback answerCallback_)
        {
            m_banner = null;
            m_connection = new Connection(ip, port);
            m_authmethod = authmethod;

            m_lasterror = null;
            m_state = State.Init;
            m_stateLock = new object();

            m_requestqueue_signal = new AutoResetEvent(false);
            m_requestqueue = new ConcurrentQueue<Request>();
            m_pendingqueue_signal = new AutoResetEvent(false);
            m_pendingqueue = new ConcurrentQueue<PendingRequest>();

            m_answerCallback = answerCallback_;
            m_request_thread = null;
            m_answer_thread = null;
        }

        public void Shutdown()
        {
            if (GetState() == State.Running)
            {
                SetState( State.Shutdown, null);
                m_requestqueue_signal.Set();
                m_pendingqueue_signal.Set();
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
                m_connection.Connect();
                byte[] ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    SetState(State.Terminated, "server closed connection");
                    return false;
                }
                m_banner = Encoding.UTF8.GetString(ln);
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
                        m_connection.WriteLine("MECH " + m_authmethod);
                        ln = m_connection.ReadLine();
                        if (ln == null) 
                        {
                            SetState(State.Terminated, "server closed connection");
                            return false;
                        }
                        else if (Protocol.IsCommand("OK", ln))
                        {
                            ///... authorized (MECHS NONE)
                            m_request_thread = new Thread( new ThreadStart(this.RunRequests));
                            m_answer_thread = new Thread( new ThreadStart(this.RunAnswers));
                            SetState( State.Running, null);
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
            m_requestqueue_signal.Set();
        }

        public int NofOpenRequests()
        {
            return m_requestqueue.Count + m_pendingqueue.Count;
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
