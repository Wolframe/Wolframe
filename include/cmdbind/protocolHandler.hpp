/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file protocol/protocolHandler.hpp
/// \brief Interface to protocol handler
#ifndef _Wolframe_CMDBIND_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_CMDBIND_PROTOCOL_HANDLER_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "system/connectionEndpoint.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {
/// \brief Forward declaration
class ExecContext;
}//namespace proc
namespace cmdbind {

/// \class ProtocolHandler
/// \brief Protocol handler interface
class ProtocolHandler
{
public:
	/// \brief Operation type
	enum Operation	{
		READ=cmdbind::CommandHandler::READ,	/// <request to read data
		WRITE=cmdbind::CommandHandler::WRITE,	/// <request to write data
		CLOSE=cmdbind::CommandHandler::CLOSE	/// <request to terminate processing
	};

	/// \brief Destructor
	virtual ~ProtocolHandler(){}

	/// \brief Set the client connection end point
	/// \param [in] remote the end point to set
	virtual void setPeer( const net::RemoteEndpointR&){}

	/// \brief Set the local connection end point
	/// \param [in] local the local point to set
	virtual void setLocalEndPoint( const net::LocalEndpointR&){}

	/// \brief Define the input buffer for processing the command
	/// \param [in] buf buffer for the data to process
	/// \param [in] allocsize allocation size of the buffer for the data to process in bytes
	virtual void setInputBuffer( void* buf, std::size_t allocsize)=0;

	/// \brief Define the input buffer for processing the command
	/// \param [in] buf buffer for the data to process
	/// \param [in] size size of the buffer for the data to process in bytes
	/// \param [in] pos cursor position in the buffer defining byte position of the start of the the data to process
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos)=0;

	/// \brief Get the next operation to do for the connection handler
	/// \return the next operation for the connection handler
	virtual Operation nextOperation()=0;

	/// \brief Passes the network input to the command handler (READ operation)
	/// \param [in] begin start of the network input block.
	/// \param [in] bytesTransferred number of bytes passed in the input block
	virtual void putInput( const void* begin, std::size_t bytesTransferred)=0;

	/// \brief Tell the protocol handler that EOF has been reached, if implemented
	virtual void putEOF(){}

	/// \brief Get the input block request (READ operation)
	/// \param [out] begin start of the network input buffer
	/// \param [out] maxBlockSize maximum size of data in bytes to pass with the subsequent putInput(const void*,std::size_t) call
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize)=0;

	/// \brief Get the next output chunk from the command handler (WRITE operation)
	/// \param [out] begin start of the output chunk
	/// \param [out] bytesToTransfer size of the output chunk to send in bytes
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer)=0;

	/// \brief Get the data left unprocessed after close. The data belongs to the caller to process.
	/// \param [out] begin returned start of the data chunk
	/// \param [out] nofBytes size of the returned data chunk in bytes
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes)=0;

	/// \brief Get the last error message of command execution to be returned to the client
	const char* lastError() const
	{
		return m_lastError.empty()?0:m_lastError.c_str();
	}

	/// \brief Set the last error message of command execution to be returned to the client
	void setLastError( const std::string& msg)
	{
		m_lastError = msg;
	}

	/// \brief Pass the reference to the execution context to the command handler
	/// \param[in] c the reference to the execution context owned by the caller (connection)
	void setExecContext( proc::ExecContext* c)
	{
		m_execContext = c;
	}

	/// \brief Get the reference to the processor provider
	/// \return the reference to the processor provider
	proc::ExecContext* execContext()
	{
		return m_execContext;
	}

	/// \brief Pass arguments to protocol handler
	virtual void setArgumentString( const std::string&){};

	/// \brief Get the termination marker to send for an abort of a running data session
	virtual const char* interruptDataSessionMarker() const	{return "";}

private:
	std::string m_lastError;				//< error operation for the client
	proc::ExecContext* m_execContext;			//< the reference to the execution context of the connection
};

typedef boost::shared_ptr<ProtocolHandler> ProtocolHandlerR;


/// \class ProtocolHandlerUnit
/// \brief Class that defines a protocol handler class and is able to create instances of it
class ProtocolHandlerUnit
{
public:
	/// \brief Get the name of the protocol implemented
	virtual const char* protocol() const=0;

	/// \brief Create an instance of this protocol handler
	virtual ProtocolHandler* createProtocolHandler()=0;
};

/// \brief Protocol handler unit reference
typedef boost::shared_ptr<ProtocolHandlerUnit> ProtocolHandlerUnitR;

}}
#endif

