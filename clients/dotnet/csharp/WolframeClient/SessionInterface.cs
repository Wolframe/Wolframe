using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WolframeClient
{
    public class Request
    {
        public int id { get; set; }          ///< id of the request
        public int number { get; set; }      ///< number of the request
        public string command { get; set; }  ///< command prefix of the request
        public string doctype { get; set; }  ///< document type of the request
        public string root { get; set; }     ///< root element of the request
        public object obj { get; set; }      ///< serializable request object
        public Type objtype { get; set; }    ///< request object type
        public Type answertype { get; set; } ///< answer object type
    };

    public class Answer
    {
        /// \brief Answer type (execution status)
        public enum MsgType {
            Error,       ///< fatal error in the request
            Failure,     ///< the request failed, the session is still alive
            Result       ///< successful execution of the request
        };
        public MsgType msgtype { get; set; } ///< status of the answer
        public int id { get; set; }          ///< id of the request
        public int number { get; set; }      ///< number of the request
        public object obj { get; set; }      ///< answer object
    };

    interface SessionInterface
    {
        /// \brief Connect to the server and do the initial
        ///        handshake with authentication
        bool Connect();
        /// \brief Signal the server to process all pending requests
        ///        and to shutdown
        void Shutdown();
        /// \brief Close the connection (abort pending requests)
        void Close();
        /// \brief Issue a request. The answer is delivered with a call 
        ///        of AnswerCallback (passed with the constructor)
        void IssueRequest(Request request);
        /// \brief Return the total number of open requests
        ///        (in the queue or already sent)
        int NofOpenRequests();
        /// \brief Get the last fatal (unrecoverable) error reported 
        string GetLastError();
    }
}
