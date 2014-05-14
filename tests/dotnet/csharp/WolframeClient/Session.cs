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
            public Type answertype { get; set; }
        };

        public class Answer
        {
            public enum MsgType { Error, Failure, Result };
            public MsgType msgtype { get; set; }
            public int id { get; set; }
            public object obj { get; set; }
        };
        public delegate void AnswerDelegate( Answer msg);

        private string m_banner;
        private AutoResetEvent m_signal;
        private Connection m_connection;
        private string m_authmethod;
        private string m_lasterror;
        private enum State { Init, Running, Shutdown, Terminated };
        private State m_state;
        private object m_stateLock;
        private Queue<Request> m_requestqueue;
        private Queue<PendingRequest> m_pendingqueue;
        private AnswerDelegate m_answerDelegate;
        private Thread m_thread;

        public Session(string ip, int port, string authmethod, AnswerDelegate answerDelegate)
        {
            m_banner = null;
            m_signal = new AutoResetEvent(false);
            m_connection = new Connection(ip, port, m_signal);
            m_authmethod = authmethod;

            m_lasterror = null;
            m_state = State.Init;
            m_stateLock = new object();

            m_requestqueue = new Queue<Request>();
            m_pendingqueue = new Queue<PendingRequest>();

            m_answerDelegate = answerDelegate;
            m_thread = null;
        }

        private void setState(State state)
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
            }
        }

        public string getLastError()
        {
            return m_lasterror;
        }

        private void HandleAnswer(PendingRequest rq)
        {
            byte[] ln = m_connection.ReadLine();
            if (ln == null)
            {
                m_lasterror = "server closed connection";
                setState( State.Terminated);
                return;
            }
            else if (Protocol.IsCommand("OK", ln))
            {
                m_answerDelegate(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, obj = null });
            }
            else if (Protocol.IsCommand("ERR", ln))
            {
                m_answerDelegate(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, obj = Protocol.CommandArg("ERR", ln) });
            }
            else if (Protocol.IsCommand("BAD", ln))
            {
                m_lasterror = "protocol error";
                setState(State.Terminated);
                return;
            }
            else if (Protocol.IsCommand("ANSWER", ln))
            {
                byte[] msg = m_connection.ReadContent();
                if (msg == null)
                {
                    m_lasterror = "server closed connection";
                    setState(State.Terminated);
                    return;
                }
                ln = m_connection.ReadLine();
                if (msg == null)
                {
                    m_lasterror = "server closed connection";
                    setState(State.Terminated);
                    return;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    object answerobj = Serializer.getResult(msg, rq.answertype);
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Result, id = rq.id, obj = answerobj });
                }
                else if (Protocol.IsCommand("ERR", ln))
                {
                    m_answerDelegate(new Answer { msgtype = Answer.MsgType.Failure, id = rq.id, obj = Protocol.CommandArg("ERR", ln) });
                }
                else if (Protocol.IsCommand("BAD", ln))
                {
                    m_lasterror = "protocol error";
                    setState(State.Terminated);
                    return;
                }
            }
        }

        private void Run()
        {
            // This method is called by Connect in an own thread
            setState( State.Running);
            while (m_state == State.Running)
            {
                m_signal.WaitOne();
                bool hasRequests = true;

                while (m_state == State.Running && (hasRequests || m_connection.HasReadData()))
                {
                    if (m_connection.HasReadData())
                    {
                        try
                        {
                            HandleAnswer( m_pendingqueue.Dequeue());
                        }
                        catch (InvalidOperationException)
                        {}
                        if (m_state != State.Running) break;
                    }
                    try
                    {
                        Request rq = m_requestqueue.Dequeue();
                        byte[] rqdata = Serializer.getRequestContent(rq.doctype, rq.root, rq.objtype, rq.obj);
                        m_connection.WriteRequest( rq.command, rqdata);
                        m_pendingqueue.Enqueue( new PendingRequest{ id=rq.id, answertype=rq.answertype});
                        m_connection.IssueReadRequest();
                    }
                    catch (InvalidOperationException)
                    {
                        hasRequests = false;
                    }
                }
            }
            ClearRequestQueue();
            if (m_state == State.Shutdown)
            {
                ProcessPendingRequests();
            }
        }

        public void Shutdown()
        {
            if (m_thread != null)
            {
                m_state = State.Shutdown;
                m_signal.Set();
                m_thread.Join();
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
                    m_lasterror = "server closed connection";
                    return false;
                }
                m_banner = Encoding.UTF8.GetString(ln);
                ln = m_connection.ReadLine();
                if (ln == null) 
                {
                    m_lasterror = "server closed connection";
                    return false;
                }
                else if (Protocol.IsCommand("OK", ln))
                {
                    m_connection.WriteLine("AUTH");
                    ln = m_connection.ReadLine();
                    if (ln == null) 
                    {
                        m_lasterror = "server closed connection";
                        return false;
                    }
                    else if (Protocol.IsCommand("MECHS", ln))
                    {
                        m_connection.WriteLine("MECH " + m_authmethod);
                        ln = m_connection.ReadLine();
                        if (ln == null) 
                        {
                            m_lasterror = "server closed connection";
                            return false;
                        }
                        else if (Protocol.IsCommand("OK", ln))
                        {
                            ///... authorized (MECHS NONE)
                            m_thread = new Thread( new ThreadStart(this.Run));
                            m_thread.Start();
                            return true;
                        }
                        else if (Protocol.IsCommand("ERR", ln))
                        {
                            m_lasterror = "authorization denied: " + Protocol.CommandArg( "ERR", ln);
                            return false;
                        }
                        else
                        {
                            m_lasterror = "authorization denied";
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
                        m_lasterror = "authorization process refused";
                        return false;
                    }
                }
                m_lasterror = "protocol error";
                return false;
            }
            catch (Exception soe)
            {
                m_lasterror = soe.Message;
                return false;
            }
        }

        public void IssueRequest( Request request)
        {
            m_requestqueue.Enqueue( request);
            m_signal.Set();
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

        private void ClearRequestQueue()
        {
            bool empty = false;
            while (!empty)
            {
                try
                {
                    Request rq = m_requestqueue.Dequeue();
                    Answer answer = new Answer { msgtype = Answer.MsgType.Error, id = rq.id, obj = "connection closed" };
                    m_answerDelegate( answer);
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
