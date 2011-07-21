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
///
///\file iprocHandler.hpp
///\brief example connection handler as processor executing lua scripts
///

#ifndef _Wolframe_iproc_HANDLER_HPP_INCLUDED
#define _Wolframe_iproc_HANDLER_HPP_INCLUDED
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "protocol.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/luaConfig.hpp"
#include "langbind/luaAppProcessor.hpp"

namespace _Wolframe {
namespace iproc {

/// The connection handler
class Connection : public net::ConnectionHandler
{
public:
	///\brief Constructor
	Connection( const net::LocalEndpoint& local, const lua::Configuration* config);

	///\brief Destructor
	virtual ~Connection();

	///\brief Set the remote peer and indicate that the connection is up now.
	///\param [in] remote remote peer
	virtual void setPeer( const net::RemoteEndpoint& remote);

	///\brief Handle a request and produce a reply (statemachine of the processor)
	virtual const net::NetworkOperation nextOperation();

	///\brief Passes the network input to the processor
	///\param [in] begin start of the network input block.
	///\param [in] bytesTransferred number of bytes passed in the input block
	///\remark Begin is ignored because it points always to the same block as given by the read network message
	virtual void networkInput( const void *begin, std::size_t bytesTransferred);

	///\brief Indicate that a timeout has occurred and the connection will be terminated
	virtual void timeoutOccured();
	///\brief Indicate that a signal has occurred and the connection will be terminated
	virtual void signalOccured();
	///\brief Indicate that an unrecoverable error has occurred and the connection will be terminated
	virtual void errorOccured( NetworkSignal);

private:
	typedef protocol::InputBlock Input;					///< input buffer type
	typedef protocol::OutputBlock Output;					///< output buffer type
	typedef protocol::InputBlock::iterator InputIterator;			///< iterator type for protocol commands

	typedef protocol::Buffer<256> LineBuffer;				///< buffer for one line of input/output
	typedef protocol::CmdParser<LineBuffer> ProtocolParser;			///< parser for the protocol
	typedef protocol::CArgBuffer<LineBuffer> ArgBuffer;			///< buffer type for the command arguments

	///\enum State
	///\brief Enumeration of processor states
	enum State
	{
		Init,				///< start state, called first time in this session
		EnterCommand,			///< parse command
		ParseArgs,			///< parse command arguments
		Processing,			///< running a command
		ProtocolError,			///< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		DiscardInput,			///< reading and discarding data until end of data has been seen
		FlushOutput,			///< state for sending end of data after flushing the output buffers after a call
		Terminate			///< terminate application processor session (close for network)
	};
	///\brief Returns the state as string for logging etc.
	///\param [in] i state to get as string
	static const char* stateName( State i)
	{
		static const char* ar[] = {"Init","EnterCommand","ParseArgs","Processing","ProtocolError","DiscardInput","FlushOutput","Terminate"};
		return ar[i];
	}
	///\enum State
	///\brief Enumeration of processor commands in the protocol after handshaking
	enum Command
	{
		empty,				///< empty line (to not get an error for no command)
		capa,				///< get the protocol capabilities
		run,				///< call a lua script
		quit				///< BYE and terminate
	};
	///\brief Returns the command name as string for instantiating the protocol command parser
	///\param [in] c the command to get as string
	static const char* commandName( Command c)
	{
		const char* ar[] = {"", "capa", "run", "quit", 0};
		return ar[c];
	}

	State m_state;				///< state of the processor (protocol main statemachine)

	LineBuffer m_buffer;			///< context (sub state) for partly parsed input lines
	ArgBuffer m_argBuffer;			///< buffer for the arguments
	Command m_cmdidx;			///< command parsed

	Input m_input;				///< buffer for network read messages
	Output m_output;			///< buffer for network write messages

	InputIterator m_itr;			///< iterator to scan protocol input
	InputIterator m_end;			///< iterator pointing to end of message buffer

	app::System m_system;			///< interface to system functions like database and loaded resources
	protocol::InputFilterR m_inputfilter;	///< network input interface for the interpreter
	protocol::FormatOutputR m_formatoutput;	///< network output interface for the interpreter
	lua::AppProcessor m_processor;		///< the interpreter state
	const char* m_functionName;		///< name of the method to execute
	bool m_functionHasIO;			///< true if the method to execute does content data processing (input/output)

	///\brief Helper function to send a line message with CRLF termination as C string
	///\param [in] str head of the line to write
	///\param [in] arg optional argument string of the line to write
	net::NetworkOperation WriteLine( const char* str, const char* arg=0);

	///\brief Passes the command data from protocol buffer to the processor
	void passInput();
};
} // namespace iproc


/// The server handler container
class ServerHandler::ServerHandlerImpl
{
public:
	ServerHandlerImpl( const HandlerConfiguration *config)
		:m_config(config){}

	net::ConnectionHandler* newConnection( const net::LocalEndpoint& local);
private:
	const HandlerConfiguration* m_config;
};

} // namespace _Wolframe
#endif

