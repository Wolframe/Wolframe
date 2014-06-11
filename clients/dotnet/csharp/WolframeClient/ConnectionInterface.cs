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
        bool Connect();
        /// \brief Close the connection
        void Close();
        /// \brief Read a line (blocking)
        byte[] ReadLine();
        /// \brief Read a message (blocking) that ends with LFdotLF or CRLFdotCRLF where LFdot sequences in the message are escaped as LFdotdot
        byte[] ReadContent();
        /// \brief Write a line (blocking)
        void WriteLine(string ln);
        /// \brief Write a content block (blocking) with correct escaping of CRLFdot and terminating with CRLFdotCRLF
        void WriteContent( byte[] content);
    }
}

