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
#ifndef _Wolframe_PROTOCOL_LINE_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_LINE_COMMAND_HANDLER_HPP_INCLUDED
#include "protocol/commandHandler.hpp"
#include "protocol.hpp"
#include "connectionHandler.hpp"
#include "countedReference.hpp"
#include <vector>
#include <string>

namespace _Wolframe {
namespace protocol {

class LineCommandHandlerBase :public CommandHandler
{
public:
	LineCommandHandlerBase();

	///\brief See Parent::setInputBuffer(void*,std::size_t,std::size_t,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos);

	///\brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

	///\brief See Parent::nextOperation()
	virtual Operation nextOperation()=0;

	///\brief See Parent::putInput(const void*,std::size_t)
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	///\brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	///\brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	///\brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	///\brief Get the error code of command execution to be returned to the client
	int statusCode() const				{return m_statusCode;}

private:
	friend class LineCommandHandler;
	InputBlock m_input;				///< buffer for network read messages
	OutputBlock m_output;				///< buffer for network write messages

	InputBlock::iterator m_itr;			///< iterator to scan protocol input
	InputBlock::iterator m_end;			///< iterator pointing to end of message buffer
};


class LineCommandHandlerSTM
{
protected:
	friend class LineCommandHandler;
	struct State
	{
		typedef int (*RunCommand)( void* data, int argc, const char** argv, OutputBlock& m_output);

		protocol::CmdParser<std::string> m_parser;
		std::vector<RunCommand> m_cmds;
		RunCommand m_runUnknown;

		State()
		{
			m_parser.add("");
		}
		void defineCommand( const char* name_, RunCommand run_)
		{
			m_parser.add( name_);
			m_cmds.push_back( run_);
		}
	};
	const std::vector<State> m_statear;
};


class LineCommandHandler :public LineCommandHandlerBase
{
public:
	///\enum CommandState
	///\brief Enumeration of command processing states
	enum CommandState
	{
		EnterCommand,			///< parse command
		ParseArgs,			///< parse command arguments
		ParseArgsEOL,			///< parse end of line after command arguments
		ProtocolError,			///< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		Terminate			///< exit and return the protocol context to the caller (CLOSED)
	};

	LineCommandHandler( const LineCommandHandlerSTM* stm_)
		:m_stm(stm_),m_argBuffer(&m_buffer),m_cmdstateidx(EnterCommand),m_stateidx(0),m_cmdidx(-1)
	{
		if (!stm_ || stm_->m_statear.size() == 0) throw std::logic_error("undefined or empty statemachine in LineCommandHandler");
	}

	virtual Operation nextOperation();

private:
	const LineCommandHandlerSTM* m_stm;			///< protocol sub state machine reference
	protocol::CArgBuffer< std::string > m_argBuffer;	///< buffer type for the command arguments
	std::string m_buffer;					///< line buffer
	CommandState m_cmdstateidx;				///< current state of command execution
	std::size_t m_stateidx;					///< current state in the STM
	int m_cmdidx;						///< index of the command to execute starting with 0 (-1 = undefined command)
};


}}
#endif

