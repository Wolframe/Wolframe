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
/// \file execProtocolHandler.hpp
/// \brief Protocol handler for the selection and execution of commands defined as a list of command handlers. Includes interpreting of commands that belong to the caller and return control to the caller, if any of them is issued. Includes also a 'Capabilities' command for showing the list of available commands
#ifndef _Wolframe_EXEC_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_EXEC_COMMAND_HANDLER_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/protocolHandler.hpp"
#include "tprocProtocolFiles.hpp"
#include "system/connectionHandler.hpp"
#include "types/countedReference.hpp"
#include "logger-v1.hpp"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class ExecProtocolHandler
/// \brief Command handler for a list of predefined protocol commands. Represents one state in the protocol statemachine.
class ExecProtocolHandler :public ProtocolHandler
{
public:
	struct Command
	{
		std::string m_cmdname;
		std::string m_procname;

		Command( const Command& o)				:m_cmdname(o.m_cmdname),m_procname(o.m_procname){}
		Command( const std::string& c, const std::string& p)	:m_cmdname(c),m_procname(p){}
		Command( const std::string& c)				:m_cmdname(c),m_procname(c){}
	};
public:
	/// \brief Constructor
	/// \param[in] rcmds_ array of commands that should return control to the caller
	/// \param[in] cmds_ array of command handlers with commands executed by this command handler
	ExecProtocolHandler( const std::vector<std::string>& rcmds_, const std::vector<Command>& cmds_);

	/// \brief Destructor
	virtual ~ExecProtocolHandler();

	/// \brief See Parent::setInputBuffer(void*,std::size_t)
	virtual void setInputBuffer( void* buf, std::size_t allocsize);

	/// \brief See Parent::setOutputBuffer(void*,std::size_t,std::size_t)
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos=0);

	/// \brief See Parent::nextOperation()
	virtual Operation nextOperation();

	/// \brief See Parent::putInput(const void*,std::size_t)
	virtual void putInput( const void *begin, std::size_t bytesTransferred);

	/// \brief See Parent::getInputBlock(void*&,std::size_t&)
	virtual void getInputBlock( void*& begin, std::size_t& maxBlockSize);

	/// \brief See Parent::getOutput(const void*&,std::size_t&)
	virtual void getOutput( const void*& begin, std::size_t& bytesToTransfer);

	/// \brief See Parent::getDataLeft(const void*&,std::size_t&)
	virtual void getDataLeft( const void*& begin, std::size_t& nofBytes);

	/// \brief Get the last command that was unknown and led to a CLOSED. If empty the command handler terminated because of an error
	const char* getCommand( int& argc, const char**& argv);

private:
	/// \enum State
	/// \brief Enumeration of processor states
	enum State
	{
		Init,				//< start state, called first time in this session
		EnterCommand,			//< parse command
		ParseArgs,			//< parse command arguments
		ParseArgsEOL,			//< parse end of line after command arguments
		Processing,			//< running a command
		FlushingOutput,			//< flushing output
		ProtocolError,			//< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		PrintCapabilities,		//< print capabilities
		Terminate			//< terminate application processor session (close for network)
	};
	/// \brief Returns the state as string for logging etc.
	/// \param [in] i state to get as string
	static const char* stateName( State i)
	{
		static const char* ar[] = {
			"Init",
			"EnterCommand",
			"ParseArgs",
			"ParseArgsEOL",
			"Processing",
			"FlushingOutput",
			"ProtocolError",
			"PrintCapabilities",
			"Terminate"
		};
		return ar[i];
	}
	State m_state;							//< processing state of the command handler
	std::string m_argBuffer;					//< buffer for the arguments
	const char* m_arg[2];						//< buffer for retured arguments

	protocol::InputBlock m_input;					//< buffer for network read messages
	protocol::OutputBlock m_output;					//< buffer for network write messages
	const char* m_writeptr;						//< message from command handler
	std::size_t m_writesize;					//< size of message from command handler in bytes
	std::size_t m_writepos;						//< position in message from command handler
	protocol::InputBlock::iterator m_itr;				//< iterator to scan protocol input
	protocol::InputBlock::iterator m_end;				//< iterator pointing to end of message buffer

	protocol::CmdParser<std::string> m_parser;			//< context dependent command parser definition
	int m_cmdidx;							//< command parsed
	int m_nofParentCmds;						//< number of commands passed by the owner to get control back
	std::vector<std::string> m_cmds;				//< list of commands available
	boost::shared_ptr<ProtocolHandler> m_protocolHandler;		//< protocol handler for currently executed command
	std::string m_capastr;						//< capability string to print
	std::size_t m_capaitr;						//< index in capabilities
};

}}
#endif

