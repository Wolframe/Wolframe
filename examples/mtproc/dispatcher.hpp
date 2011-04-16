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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///
/// \file dispatcher.hpp
/// \brief implements parsing and wrapping command calls to the application processing
///
#ifndef _Wolframe_METHODTABLE_COMMAND_DISPATCHER_PRIVATE_HPP_INCLUDED
#define _Wolframe_METHODTABLE_COMMAND_DISPATCHER_PRIVATE_HPP_INCLUDED
#include "protocol.hpp"
#include "methodtable.hpp"
#include <exception>

namespace _Wolframe {
namespace mtproc {

struct CommandDispatcher
{
public:
	enum Command {unknown=-1, empty=0, capa, quit, method};

private:
	//exception thrown in case of an illegal state (internal error, must not happen)
	struct IllegalState :public std::logic_error
	{
		IllegalState() :std::logic_error( "IllegalState in CommandDispatcher") {}
	};

	enum State
	{
		Null,				//< not instantiated yet
		Init,				//< object and method table have been initialized
		Parsing,			//< parsing command
		Selected,			//< command has been parsed
		ArgumentsParsed,		//< command arguments have been parsed
		Running				//< running command, interrupted by a yield
	};
	static const char* stateName( State s) {const char* ar[]={"Null","Init","Parsing","Selected","ArgumentsParsed","Running"}; return ar[s];}


	typedef protocol::CmdBuffer CmdBuffer;			//< buffer type for protocol commands
	typedef protocol::CmdParser<CmdBuffer> ProtocolParser;	//< parser type for the protocol
	typedef protocol::Buffer<256> LineBuffer;		//< buffer type for the command argument strings
	typedef protocol::CArgBuffer<LineBuffer> ArgBuffer;	//< buffer type for the command arguments

	CmdBuffer m_cmdBuffer;			//< buffer for the command
	LineBuffer m_lineBuffer;		//< buffer for the argument strings
	ArgBuffer m_argBuffer;			//< buffer for the arguments
	ProtocolParser m_parser;		//< parser for the commands
	Instance* m_instance;			//< method table and data
	Method::Context m_context;		//< context of current method executed
	unsigned int m_methodIdx;		//< index of currently executed method or -1
	Command m_command;			//< command parsed (return value of getCommand)
	State m_state;				//< command handler state
	unsigned int m_argc;			//< number of arguments parsed
	const char** m_argv;			//< pointer to arguments parsed

	//set the current command to unknown without affecting the processed data object
	void resetCommand();

	//initialize the processed data object with its methods and set the current command to unknown
	void init( const char** protocolCmds, Instance* instance);

public:
	//initialize the prcessed data object with all its methods and some default protocol commands
	void init( Instance* instance)
	{
		static const char* cmd[4] = {"","capa","quit", 0};
		init( cmd, instance);
	}

	CommandDispatcher( Instance* instance=0);

	//return the type of the command
	Command getCommand( protocol::InputBlock::iterator& itr, protocol::InputBlock::iterator& eoM);

	//call the function (first or subsequent call)
	enum IOState {ReadInput,WriteOutput,Close,Error};
	IOState call( int& returnCode);

	//pass a protocol data input chunk to the processors generator function
	void protocolInput( protocol::InputBlock::iterator& start, protocol::InputBlock::iterator& end, bool eoD);

	bool getOutput( void** output, unsigned int* outputsize);
	void setOutputBuffer( void* buf, unsigned int bufsize);

	//get the capabilities message
	const char* getCapabilities();

	//return true, if the currently executed command has I/O
	bool commandHasIO() const;
};

}}//namespace
#endif


