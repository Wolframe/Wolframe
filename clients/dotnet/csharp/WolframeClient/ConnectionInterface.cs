using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

/*
 * Wolframe Client Connection Interface
 * 
 * Connection constructor: Connection( string ipaddress, int port)
 */

namespace WolframeClient
{
    interface ConnectionInterface
    {
        /// \brief Connect to the server via TCP plain or SSL
        void Connect();
        /// \brief Close the connection
        void Close();
        /// \brief Test if there is data to read
        bool HasReadData();
        /// \brief Read a line (blocking)
        byte[] ReadLine();
        /// \brief Read a message (blocking) that ends with LFdotLF or CRLFdotCRLF where LFdot sequences in the message are escaped as LFdotdot
        byte[] ReadContent();
        /// \brief Write a line
        void WriteLine(string ln);
        /// \brief Write a resquest
        void WriteRequest(string command, byte[] content);
    }
}

