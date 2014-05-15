using System;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Threading;
using System.Net.Sockets;
using WolframeClient;

namespace WolframeClient
{
    class Session
    {
        public class Request
        {
            public int id { get; set; }
            public int number { get; set; }
            public string command { get; set; }
            public string doctype { get; set; }
            public string root { get; set; }
            public object obj { get; set; }
            public Type objtype { get; set; }
            public Type answertype { get; set; }
        };

        private class PendingRequest
        {
            public int id { get; set; }
            public int number { get; set; }
            public Type answertype { get; set; }
        };

        public class Answer
        {
            public enum MsgType { Error, Failure, Result };
            public MsgType msgtype { get; set; }
            public int id { get; set; }
            public int number { get; set; }
            public object obj { get; set; }
        };
        public delegate void AnswerDelegate( Answer msg);

        private string m_banner;
        private Connection m_connection;
        private string m_authmethod;
        private string m_lasterror;
        private enum State { Init, Running, Shutdown, Terminated };
        private State m_state;
        private object m_stateLock;
        private AutoResetEvent m_requestqueue_signal;
        private Queue<Request> m_requestqueue;
        private AutoResetEvent m_pendingqueue_signal;
        private Queue<PendingRequest> m_pendingqueue;
        private AnswerDelegate m_answerDelegate;
        private Thread m_request_thread;
        private Thread m_answer_thread;

        public Session(string ip, int port, string authmethod, AnswerDelegate answerDelegate)
        {
            m_banner = null;
            m_connection = new Connection(ip, port);
            m_authmethod = authmethod;

            m_lasterror = null;
            m_state = State.Init;
            m_stateLock = new object();

            m_requestqueue_signal = new AutoResetEvent(false);
            m_requestqueue = new Queue<Request>();
            m_pendingqueue_signal = new AutoResetEvent(false);
            m_pendingqueue = new Queue<PendingRequest>();

            m_answerDelegate = answerDelegate;
            m_request_thread = null;
            m_answer_thread = null;
        }

        private void setState(State state, string err)
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

        public string getLastError()
        {
            lock (m_stateLock)
            {
                string err = m_lasterror;
                m_lasterror = null;
                return err;
            }
        }

        private void HandleAnswer(PendingRequest rq)
        {
            byte[] ln = m_connection.ReadLine();
            if (ln == null)
            {
                setState(State.Terminated, "server closed connection");
                return;
            }
            else if (Protocol.IsCommand("OK", ln))
            {
                m_answerDelegate(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, number=rq.number, obj = null });
            }
            else if (Protocol.IsCommand("ERR", ln))
            {
                m_answerDelegate(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, number = rq.number, obj = Protocol.CommandArg("ERR", ln) });
            }
            else if (Protocol.IsCommand("BAD", ln))
            {
                setState(State.Terminated, "protocol error");
                m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "protocol error" });
                return;
            }
            else if (Protocol.IsCommand("ANSWER", ln))
            {
                byte[] msg = m_connection.ReadContent();
                if (msg == null)
                {
                    setState(State.Terminated, "server closed connection");
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "server closed connection" });
                    return;
                }
                ln = m_connection.ReadLine();
                if (msg == null)
                {
                    setState(State.Terminated, "server closed connection");
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "server closed connection" });
                    return;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    object answerobj = Serializer.getResult(msg, rq.answertype);
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, number = rq.number, obj = answerobj });
                }
                else if (Protocol.IsCommand("ERR", ln))
                {
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, number = rq.number, obj = Protocol.CommandArg("ERR", ln) });
                }
                else if (Protocol.IsCommand("BAD", ln))
                {
                    setState(State.Terminated, "protocol error");
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = "protocol error" });
                    return;
                }
            }
        }

        private void RunRequests()
        {
            // This method is called by Connect in an own thread and processes the write of requests to the server
            while (m_state == State.Running)
            {
                try
                {
                    Request rq = m_requestqueue.Dequeue();
                    byte[] rqdata = Serializer.getRequestContent(rq.doctype, rq.root, rq.objtype, rq.obj);
                    m_pendingqueue.Enqueue(new PendingRequest { id = rq.id, number = rq.number, answertype = rq.answertype });
                    m_connection.WriteRequest( rq.command, rqdata);
                    m_pendingqueue_signal.Set();
                }
                catch (InvalidOperationException)
                {
                    m_requestqueue_signal.WaitOne();
                }
            }
            ClearRequestQueue();
        }

        private void RunAnswers()
        {
            // This method is called by Connect in an own thread and processes the reading of answers from the server
            setState( State.Running, null);
            while (m_state == State.Running)
            {
                try
                {
                    HandleAnswer( m_pendingqueue.Dequeue());
                }
                catch (InvalidOperationException)
                {
                    m_pendingqueue_signal.WaitOne();
                }
            }
            if (m_state == State.Shutdown)
            {
                ProcessPendingRequests();
            }
        }

        public void Shutdown()
        {
            if (m_state == State.Running)
            {
                setState( State.Shutdown, null);
                m_pendingqueue_signal.Set();
                m_requestqueue_signal.Set();
                m_connection.Close();
                m_request_thread.Join();
                m_answer_thread.Join();
            }
        }

        public void Abort()
        {
            setState(State.Terminated, "session aborted");
            m_requestqueue_signal.Set();
            m_pendingqueue_signal.Set();
            m_request_thread.Join();
            m_answer_thread.Join();
        }

        public bool Connect()
        {
            try
            {
                m_connection.Connect();
                byte[] ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    setState(State.Terminated, "server closed connection");
                    return false;
                }
                m_banner = Encoding.UTF8.GetString(ln);
                ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    setState(State.Terminated, "server closed connection");
                    return false;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    m_connection.WriteLine("AUTH");
                    ln = m_connection.ReadLine();
                    if (ln == null) 
                    {
                        setState(State.Terminated, "server closed connection");
                        return false;
                    }
                    else if (Protocol.IsCommand("MECHS", ln))
                    {
                        m_connection.WriteLine("MECH " + m_authmethod);
                        ln = m_connection.ReadLine();
                        if (ln == null) 
                        {
                            setState(State.Terminated, "server closed connection");
                            return false;
                        }
                        else if (Protocol.IsCommand("OK", ln))
                        {
                            ///... authorized (MECHS NONE)
                            m_request_thread = new Thread( new ThreadStart(this.RunRequests));
                            m_answer_thread = new Thread( new ThreadStart(this.RunAnswers));
                            setState( State.Running, null);
                            m_request_thread.Start();
                            m_answer_thread.Start();
                            return true;
                        }
                        else if (Protocol.IsCommand("ERR", ln))
                        {
                            setState(State.Terminated, "authorization denied: " + Protocol.CommandArg( "ERR", ln));
                            return false;
                        }
                        else
                        {
                            setState(State.Terminated, "authorization denied");
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
                        setState(State.Terminated, "authorization process refused");
                        return false;
                    }
                }
                setState(State.Terminated, "protocol error");
                return false;
            }
            catch (Exception soe)
            {
                setState(State.Terminated, soe.Message);
                return false;
            }
        }

        public void IssueRequest( Request request)
        {
            m_requestqueue.Enqueue( request);
            m_requestqueue_signal.Set();
        }

        private void ProcessPendingRequests()
        {
            bool empty = false;
            while (!empty && m_state != State.Terminated)
            {
                try
                {
                    HandleAnswer(m_pendingqueue.Dequeue());
                }
                catch (InvalidOperationException)
                {
                    //... done
                    empty = true;
                }
            }
        }

        TODO: MAKE QUEUES THREAD SAFE, THEY ARE NOT

        private void ClearRequestQueue()
        {
            bool empty = false;
            while (!empty)
            {
                try
                {
                    Request rq = m_requestqueue.Dequeue();
                    if (m_lasterror != null)
                    {
                        string msg = "session terminated: " + m_lasterror;
                        m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = msg });
                    }
                    else
                    {
                        string msg = "session terminated";
                        m_answerDelegate(new Answer { msgtype = Answer.MsgType.Error, id = rq.id, number = rq.number, obj = msg });
                    }
                }
                catch (InvalidOperationException)
                {
                    //... done
                    empty = true;
                }
            }
        }
    };
}
