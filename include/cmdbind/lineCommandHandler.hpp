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
//\file cmdbind/lineCommandHandler.hpp
//\brief Interface to a generic command handler for a networkHandler command with delegation of network I/O until the command context is left
#ifndef _Wolframe_cmdbind_LINE_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_cmdbind_LINE_COMMAND_HANDLER_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "protocol/ioblocks.hpp"
#include "protocol/parser.hpp"
#include "system/connectionHandler.hpp"
#include "types/countedReference.hpp"
#include "logger-v1.hpp"
#include <vector>
#include <string>
#include <iostream>

namespace _Wolframe {
namespace cmdbind {

//\brief State machine definition of a LineCommandHandler
class LineCommandHandlerSTM
{
public:
	//\return -1 for terminate or a valid state in the state machine definition
	typedef int (*RunCommand)( void* obj, int argc, const char** argv, std::ostream& out);
	struct State
	{
		protocol::CmdParser<std::string> m_parser;
		std::vector<RunCommand> m_cmds;

		State()
		{
			m_parser.add(""); ///... the empty line as command is added implicitely to every state as command with index 0 and handled as such in the STM
		}
		void defineCommand( const char* name_, RunCommand run_)
		{
			m_parser.add( name_);
			m_cmds.push_back( run_);
		}
	};

	int runCommand( std::size_t stateidx, std::size_t cmdidx, void* obj, int argc, const char** argv, std::ostream& out) const
	{
		const State& st = get( stateidx);
		if (cmdidx >= st.m_cmds.size()) throw std::logic_error( "illegal command reference");
		return st.m_cmds.at(cmdidx)( obj, argc, argv, out);
	}

	void defineState( std::size_t se)
	{
		if (se != m_statear.size()) throw std::logic_error( "state index defined not matching to enum given as state index");
		State st;
		m_statear.push_back( st);
	}

	void defineCommand( const char* name_, RunCommand run_)
	{
		m_statear.back().defineCommand( name_, run_);
	}

	const State& get( std::size_t idx) const
	{
		if (idx >= m_statear.size()) throw std::logic_error( "illegal state");
		return m_statear.at( idx);
	}
private:
	std::vector<State> m_statear;
};


class LineCommandHandler :public CommandHandler
{
public:
	//\brief Constructor
	explicit LineCommandHandler( const LineCommandHandlerSTM* stm_, std::size_t stateidx_=0);
	//\brief Destructor
	virtual ~LineCommandHandler();

	//\brief See Parent::setInputBuffer(void*,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	//\brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos=0);

	//\brief See Parent::nextOperation()
	virtual Operation nextOperation();

	//\brief See Parent::putInput(const void*,std::size_t)
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	//\brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	//\brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	//\brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	//\brief See Parent::interruptDataSessionMarker()
	virtual const char* interruptDataSessionMarker() const;

	//\brief Get the current state (toplevel)
	std::size_t stateidx() const			{return m_stateidx;}

	const std::vector<std::string>& cmds() const	{return m_stm->get( m_stateidx).m_parser.cmds();}
	static const char* endl()			{return "\r\n";}

protected:
	typedef int (*DelegateHandlerEnd)( void*, CommandHandler*, std::ostream&);

	//\brief Delegate the processing to 'ch' until its termination. Call the processing termination function for informing the caller
	void delegateProcessingFunction( CommandHandler* ch, DelegateHandlerEnd end)
	{
		if (m_delegateHandler) throw std::logic_error( "duplicate delegation of protocol processing");
		m_delegateHandler = ch;
		m_delegateHandlerEnd = end;
	}

	//\brief Redirect to another command of the state machine.
	//\remark The command cmd_ must exist, though it may fail
	//\param[in] cmd_ command of the current state to execute
	//\param[in] argc_ number of arguments to pass to the command
	//\param[in] argv_ the arguments to pass to the command
	//\param[out] out stream for protocol output of the command
	int runCommand( const char* cmd_, int argc_, const char** argv_, std::ostream& out);


	//\brief Redirect data as input and the output buffer to a command handler
	//\param[in] data pointer to input data for the command handler
	//\param[in] datasize size of 'data' in bytes
	//\param[in] toh command handler to address
	//\param[in,out] out buffer for protocol write
	bool redirectInput( void* data, std::size_t datasize, cmdbind::CommandHandler* toh, std::ostream& out);

private:
	protocol::InputBlock m_input;				//< buffer for network read messages
	protocol::OutputBlock m_output;				//< buffer for network write messages

	protocol::InputBlock::iterator m_itr;			//< iterator to scan protocol input
	protocol::InputBlock::iterator m_end;			//< iterator pointing to end of message buffer

	//\enum CommandState
	//\brief Enumeration of command processing states
	enum CommandState
	{
		Init,						//< start state, called first time in this session
		ProcessingDelegation,				//< the command execution has been delegated to another command handler
		EnterCommand,					//< parse command
		ParseArgs,					//< parse command arguments
		ParseArgsEOL,					//< parse end of line after command arguments
		ProtocolError,					//< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		ProcessOutput,					//< prints the command output to the output buffer
		Terminate					//< terminate application processor session (close for network)
	};
	//\brief Returns the state as string for logging etc.
	//\param [in] i state to get as string
	static const char* commandStateName( CommandState i)
	{
		static const char* ar[] = {"Init","ProcessingDelegation","EnterCommand","ParseArgs","ParseArgsEOL","ProtocolError","ProcessOutput","Terminate"};
		return ar[i];
	}
	CommandHandler* m_delegateHandler;			//< command handler that processes the I/O delegated from this command handler until it return a CLOSE and gets destroyed again
	DelegateHandlerEnd m_delegateHandlerEnd;		//< function type called after termination of m_delegateHandler
	const LineCommandHandlerSTM* m_stm;			//< command level protocol state machine
	protocol::CArgBuffer< std::string > m_argBuffer;	//< buffer type for the command arguments
	std::string m_buffer;					//< line buffer
	CommandState m_cmdstateidx;				//< current state of command execution
	std::size_t m_stateidx;					//< current state in the STM
	int m_cmdidx;						//< index of the command to execute starting with 0 (-1 = undefined command)
	int m_resultstate;					//< result state of the last command
	std::string m_resultstr;				//< content the command output stream after command execution
	std::size_t m_resultitr;				//< iterator on m_result to send the output via network output
};


//\brief Defines a static function calling a member function with fixed signature
//\warning do not declare virtual method calls like this. It is not portable (GCC only) !
//PF:TODO make a BOOST_ASSERT here for refusing virtual methods here
template <class T, int (T::*TerminateDelegationMethod)( CommandHandler*, std::ostream&)>
struct LineCommandHandlerTerminateDelegationWrapper
{
	static int function( void* this_, CommandHandler* ch, std::ostream& out)
	{
		return (((T*)this_)->*TerminateDelegationMethod)( ch, out);
	}
};

//\brief Defines some template based extensions to line command handler
// Usage: derive LineCommandHandlerImpl from LineCommandHandlerTemplate<LineCommandHandlerImpl>
template <class LineCommandHandlerImpl>
struct LineCommandHandlerTemplate :public LineCommandHandler
{
	//\brief Constructor
	explicit LineCommandHandlerTemplate( const LineCommandHandlerSTM* stm_, std::size_t stateidx_=0)
		:LineCommandHandler( stm_,stateidx_){}

	//\brief Destructor
	virtual ~LineCommandHandlerTemplate(){}

	//\brief Delegate the processing to 'ch' until its termination. Call the processing termination function for informing the caller
	template <int (LineCommandHandlerImpl::*EndDelegateProcessingMethod)( CommandHandler*, std::ostream&)>
	void delegateProcessing( CommandHandler* ch)
	{
		delegateProcessingFunction( ch, &LineCommandHandlerTerminateDelegationWrapper<LineCommandHandlerImpl,EndDelegateProcessingMethod>::function);
	}
};


//\brief Defines a static function calling a member function with fixed signature
//\warning do not declare virtual method calls like this. It is not portable (GCC only) !
//PF:TODO make a BOOST_ASSERT here for refusing virtual methods here
template <class T, int (T::*Method)( int argc, const char** argv, std::ostream& out)>
struct LineCommandHandlerWrapper
{
	static int function( void* this_, int argc, const char** argv, std::ostream& out)
	{
		return (((T*)this_)->*Method)( argc, argv, out);
	}
};


//\class LineCommandHandlerSTMTemplate
//\brief Template to build up a protocol description out of basic bricks
template <class LineCommandHandlerImpl>
struct LineCommandHandlerSTMTemplate :public LineCommandHandlerSTM
{
	LineCommandHandlerSTMTemplate& operator[]( std::size_t se)
	{
		defineState( se);
		return *this;
	}

	template <int (LineCommandHandlerImpl::*Method)(int,const char**,std::ostream&)>
	LineCommandHandlerSTMTemplate& cmd( const char* cmdname_)
	{
		defineCommand( cmdname_, &LineCommandHandlerWrapper<LineCommandHandlerImpl,Method>::function);
		return *this;
	}
};

}}
#endif

