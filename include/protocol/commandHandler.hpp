/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
///\file protocol/commandHandler.hpp
///\brief interface to a generic command handler for a networkHandler command with delegation of network I/O until the command context is left
#ifndef _Wolframe_PROTOCOL_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_COMMAND_HANDLER_HPP_INCLUDED
#include "connectionHandler.hpp"
#include "countedReference.hpp"
#include <vector>
#include <string>
#include <stdexcept>

namespace _Wolframe {
namespace protocol {

class CommandHandler
{
public:
	///\brief typedef for private net::NetworkOperation::Operation
	enum Operation
	{
		READ,
		WRITE,
		CLOSED
	};

	CommandHandler()
		:m_statusCode(0){}

	///\brief Pass the parameters for processing
	///\param [in] argc number of arguments of the protocol command
	///\param [in] argv arguments of the protocol command
	void passParameters( int argc, const char** argv);

	///\brief Define the input buffer for processing the command
	///\param [in] buf buffer for the data to process
	///\param [in] allocsize allocation size of the buffer for the data to process in bytes
	///\param [in] size size of the data filled in the in bytes
	///\param [in] itrpos cursor position in the buffer defining byte position of the start of the the data to process
	virtual void setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos)=0;

	///\brief Define the input buffer for processing the command
	///\param [in] buf buffer for the data to process
	///\param [in] size size of the buffer for the data to process in bytes
	///\param [in] pos cursor position in the buffer defining byte position of the start of the the data to process
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos)=0;

	///\brief Get the next operation to do for the connection handler
	///\return the next operation for the connection handler
	virtual Operation nextOperation()=0;

	///\brief Passes the network input to the command handler (READ operation)
	///\param [in] begin start of the network input block.
	///\param [in] bytesTransferred number of bytes passed in the input block
	virtual void putInput( const void *begin, std::size_t bytesTransferred)=0;

	///\brief Get the input block request (READ operation)
	///\param [out] begin start of the network input buffer
	///\param [out] maxBlockSize maximum size of data in bytes to pass with the subsequent putInput(const void*,std::size_t) call
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize)=0;

	///\brief Get the next output chunk from the command handler (WRITE operation)
	///\param [out] begin start of the output chunk
	///\param [out] bytesToTransfer size of the output chunk to send in bytes
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer)=0;

	///\brief Get the data left unprocessed after close. The data belongs to the caller to process.
	///\param [out] begin returned start of the data chunk
	///\param [out] nofBytes size of the returned data chunk in bytes
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes)=0;

	///\brief Get the error code of command execution to be returned to the client
	int statusCode() const				{return m_statusCode;}

protected:
	std::vector< std::string > m_argBuffer;		///< buffer type for the command arguments
	int m_statusCode;				///< error code of operation for the client
};


typedef CountedReference<CommandHandler> CommandHandlerR;


struct CommandConfig
{
	CommandConfig(){}
	virtual ~CommandConfig(){}

	///\brief interface implementation of ConfigurationBase::test() const
	virtual bool test() const=0;

	///\brief interface implementation of ConfigurationBase::check() const
	virtual bool check() const=0;

	///\brief interface implementation of ConfigurationBase::print(std::ostream& os, size_t indent) const
	virtual void print( std::ostream&, size_t indent=0) const=0;
};


class CommandBase
{
public:
	CommandBase( const std::string& nam, const CommandConfig* cfg)
		:m_protocolCmdName(nam),m_config(cfg){}
	CommandBase( const CommandBase& o)
		:m_protocolCmdName(o.m_protocolCmdName),m_config(o.m_config){}
	CommandBase()
		:m_protocolCmdName(0),m_config(0){}
	virtual ~CommandBase() {}

	const char* protocolCmdName() const
	{
		return m_protocolCmdName.c_str();
	}

	virtual CommandHandlerR create( int /*argc*/, const char** /*argv*/) const
	{
		return CommandHandlerR(0);
	}

	const CommandConfig* config() const
	{
		return m_config;
	}

protected:
	std::string m_protocolCmdName;
	const CommandConfig* m_config;
};


template <class CommandHandlerClass, class CommandConfigClass>
struct Command :public CommandBase
{
	Command( const char* nam, const CommandConfigClass* cfg)
		:CommandBase(nam, dynamic_cast<const CommandConfig*>(cfg))
	{
		if (!dynamic_cast<const CommandConfigClass*>(m_config))
		{
			throw std::logic_error( "Base class mismatch or RTTI support is switched off");
		}
	}
	virtual ~Command(){}

	virtual CommandHandlerR create( int argc, const char** argv) const
	{
		CommandHandlerClass* rt = new CommandHandlerClass( dynamic_cast<const CommandConfigClass*>(m_config));
		rt->passParameters( argc, argv);
		return CommandHandlerR( rt);
	}
};

}}
#endif

