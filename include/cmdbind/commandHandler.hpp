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
/// \file cmdbind/commandHandler.hpp
/// \brief Interface to a generic command handler
#ifndef _Wolframe_CMDBIND_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_CMDBIND_COMMAND_HANDLER_HPP_INCLUDED
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "types/doctypeinfo.hpp"
#include "processor/procProviderInterface.hpp"

namespace _Wolframe {

namespace proc {
/// \brief Forward declaration
class ExecContext;
}//namespace proc

namespace cmdbind {

/// \class CommandHandler
/// \brief Command handler interface
class CommandHandler
{
public:
	/// \brief Operation type
	enum Operation	{
		READ,			/// <request to read data
		WRITE,			/// <request to write data
		CLOSE			/// <request to terminate processing
	};

	/// \brief Defaul constructor
	CommandHandler()
		:m_execContext(0){}

	/// \brief Destructor
	virtual ~CommandHandler(){}

	/// \brief Get the next operation to do for the connection handler
	/// \return the next operation for the connection handler
	virtual Operation nextOperation()=0;

	/// \brief Passes the network input to the command handler (READ operation)
	/// \param [in] begin start of the network input block.
	/// \param [in] bytesTransferred number of bytes passed in the input block
	/// \param [in] eod (end of data) true, if the passed chunk is the last one
	virtual void putInput( const void* begin, std::size_t bytesTransferred, bool eod)=0;

	/// \brief Get the next output chunk from the command handler (WRITE operation)
	/// \param [out] begin start of the output chunk
	/// \param [out] bytesToTransfer size of the output chunk to send in bytes
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer)=0;


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
	virtual void setExecContext( proc::ExecContext* c)
	{
		m_execContext = c;
	}

	/// \brief Get the reference to the processor provider
	/// \return the reference to the processor provider
	proc::ExecContext* execContext()
	{
		return m_execContext;
	}

	/// \brief Get info about processed document type
	const types::DoctypeInfo* doctypeInfo() const
	{
		return m_doctypeinfo.get();
	}

	/// \brief Set info about processed document type
	virtual void setDoctypeInfo( const types::DoctypeInfoR& doctypeinfo_)
	{
		m_doctypeinfo = doctypeinfo_;
	}

	/// \brief Get a hint for the size of output chunks in bytes
	std::size_t outputChunkSize() const
	{
		return m_outputChunkSize;
	}

	/// \brief Set a hint for the size of output chunks in bytes
	virtual void setOutputChunkSize( std::size_t outputChunkSize_)
	{
		m_outputChunkSize = outputChunkSize_;
	}

private:
	std::string m_lastError;		///< Error operation for the client
	proc::ExecContext* m_execContext;	///< The reference to the execution context of the connection
	types::DoctypeInfoR m_doctypeinfo;	///< Document type information
	std::size_t m_outputChunkSize;		///< Hint for the size of output chunks
};

typedef boost::shared_ptr<CommandHandler> CommandHandlerR;


/// \class CommandHandlerUnit
/// \brief Class that defines a command handler class and is able to create instances of it
class CommandHandlerUnit
{
public:
	/// \brief Destructor
	virtual ~CommandHandlerUnit(){}

	/// \brief Load all configured programs
	virtual bool loadPrograms( const proc::ProcessorProviderInterface* provider)=0;

	/// \brief Get the list of all commands inplemented by this command handler
	virtual std::vector<std::string> commands() const=0;

	/// \brief Create an instance of this command handler
	virtual CommandHandler* createCommandHandler( const std::string& cmdname, const std::string& docformat)=0;
};

/// \brief Command handler unit reference
typedef boost::shared_ptr<CommandHandlerUnit> CommandHandlerUnitR;


}}//namespace
#endif

