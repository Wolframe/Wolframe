using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/*
 * Wolframe Client Session Interface 
 * 
 * Session constructor: Session( string ipaddress, int port, string authmethod, AnswerCallback answerCallback)
 * The session class uses WolframeClient.Connection for the server connection
 */

namespace WolframeClient
{
    public class Request
    {
        public int id { get; set; }                        ///< id of the request passed by the caller and returned in the answer to identify the request type
        public int number { get; set; }                    ///< number of the request passed by the caller and returned in the answer to possibly distinguish different requests of the same kind (id)
        public string command { get; set; }                ///< command prefix of the request
        public string doctype { get; set; }                ///< document type of the request
        public string root { get; set; }                   ///< root element of the request
        public object obj { get; set; }                    ///< serializable object of the request
        public Type objtype { get; set; }                  ///< type of the object of the request
        public Type answertype { get; set; }               ///< type of the object of the answer to be returned
    };

    public class Answer
    {
        /// \brief Answer type (execution status)
        public enum MsgType {
            Error,                                         ///< fatal error in the request
            Failure,                                       ///< server returned error, the command could not be executed but the session is still alive
            Result                                         ///< successful execution and null for empty or a valid answer object returned
        };
        public MsgType msgtype { get; set; }               ///< status of the answer
        public int id { get; set; }                        ///< id of the request (as defined in Request)
        public int number { get; set; }                    ///< number of the request (as defined in Request)
        public object obj { get; set; }                    ///< answer object
    };
    public delegate void AnswerCallback(Answer msg);


    interface SessionInterface
    {
        /// \brief Connect to the server and do the initial handshake with authentication
        bool Connect();
        /// \brief Signal the server to process all pending (already sent requests) and to shutdown
        void Shutdown();
        /// \brief Close the connection (abort pending requests)
        void Close();
        /// \brief Issue a request. The answer is delivered with a call of AnswerCallback (passed with the constructor)
        void IssueRequest(Request request);
        /// \brief Return the total number of open requests (in the queue or already sent)
        int NofOpenRequests();
        /// \brief Get the last fatal (unrecoverable session) error reported 
        string GetLastError();
    }
}
