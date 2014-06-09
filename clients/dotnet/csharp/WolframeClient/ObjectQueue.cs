using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace WolframeClient
{
    class WorkQueue<T>
    {
        public enum State { Active, Done };
        private Queue<T> m_impl;
        private object m_mutex;
        private AutoResetEvent m_signal;
        private State m_state;
        private char m_objtype;
        private volatile bool m_hasElement;

        public int Count()
        {
            lock (m_mutex)
            {
                return m_impl.Count;
            }
        }

        public WorkQueue( char objtype)
        {
            m_impl = new Queue<T>();
            m_mutex = new object();
            m_signal = new AutoResetEvent(true);
            m_state = State.Active;
            m_objtype = objtype;
            m_hasElement = true;
        }

        public bool Enqueue(T obj)
        {
            lock (m_mutex)
            {
                if (m_state == State.Done) return false;
                m_hasElement = true;
                m_impl.Enqueue(obj);
                Monitor.Pulse( m_mutex);
            }
            return true;
        }

        public bool Dequeue( out T result)
        {
            while (true)
            {
                lock (m_mutex)
                {
                    m_hasElement = (m_impl.Count > 1);
                    if (m_state == State.Done)
                    {
                        result = default(T);
                        return false;
                    }
                    try
                    {
                        result = m_impl.Dequeue();
                        return true;
                    }
                    catch (InvalidOperationException)
                    {
                        Monitor.Wait(m_mutex);
                    }
                }
            }
        }

        public bool DequeueUnblocking( out T result)
        {
            try
            {
                lock (m_mutex)
                {
                    result = m_impl.Dequeue();
                    return true;
                }
            }
            catch (InvalidOperationException)
            {
                result = default(T);
                return false;
            };
        }

        public void Terminate()
        {
            lock (m_mutex)
            {
                m_state = State.Done;
                Monitor.Pulse(m_mutex);
            }
        }
    }
}
