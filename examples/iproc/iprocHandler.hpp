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
///\file iprocHandler.hpp
///\brief Example connection handler with simple protocol

#ifndef _Wolframe_iproc_HANDLER_HPP_INCLUDED
#define _Wolframe_iproc_HANDLER_HPP_INCLUDED
#include "system/connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "cmdbind/commandHandler.hpp"
#include "filter/filter.hpp"
#include "protocol/ioblocks.hpp"
#include "protocol/parser.hpp"
#include "types/countedReference.hpp"

namespace _Wolframe {
namespace iproc {

//\class Connection
//\brief The connection handler
class Connection : public net::ConnectionHandler
{
public:
	///\brief Constructor
	Connection( const net::LocalEndpoint& local, const Configuration* config);

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

	///\brief Indicate that an unrecoverable error, a timeout or a terminate signal has occurred and the connection will be terminated
	virtual void signalOccured( NetworkSignal);

	///\brief Set the reference to the prcessor provider
	void setProcessorProvider( proc::ProcessorProvider* provider_)
	{
		m_provider = provider_;
	}

private:
	///\enum State
	///\brief Enumeration of processor states
	enum State
	{
		Init,				//< start state, called first time in this session
		EnterCommand,			//< parse command
		ParseArgs,			//< parse command arguments
		ParseArgsEOL,			//< parse end of line after command arguments
		Processing,			//< running a command
		ProtocolError,			//< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		Terminate			//< terminate application processor session (close for network)
	};
	///\brief Returns the state as string for logging etc.
	///\param [in] i state to get as string
	static const char* stateName( State i)
	{
		static const char* ar[] = {"Init","EnterCommand","ParseArgs","ParseArgsEOL","Processing","ProtocolError","Terminate"};
		return ar[i];
	}
	///\enum State
	///\brief Enumeration of processor commands in the protocol after handshaking
	enum Command
	{
		empty,				//< empty line (to not get an error for no command)
		capa,				//< get the protocol capabilities
		quit				//< BYE and terminate

	};
	enum {NofCommands=3};

	///\brief Returns the command name as string for instantiating the protocol command parser
	///\param [in] c the command to get as string
	static const char* commandName( Command c)
	{
		const char* ar[] = {"", "capa", "quit", 0};
		return ar[c];
	}

	State m_state;								//< state of the processor (protocol main statemachine)

	protocol::Buffer m_buffer;						//< context (sub state) for partly parsed input lines
	protocol::CArgBuffer<protocol::Buffer> m_argBuffer;			//< buffer for the arguments

	protocol::InputBlock m_input;						//< buffer for network read messages
	protocol::OutputBlock m_output;						//< buffer for network write messages

	langbind::InputFilterR m_inputfilter;
	langbind::OutputFilterR m_outputfilter;

	protocol::InputBlock::iterator m_itr;					//< iterator to scan protocol input
	protocol::InputBlock::iterator m_end;					//< iterator pointing to end of message buffer

	const Configuration* m_config;						//< configuration
	protocol::CmdParser<protocol::Buffer> m_parser;				//< context dependent command parser definition
	int m_cmdidx;								//< command parsed
	types::CountedReference<cmdbind::CommandHandler> m_cmdhandler;		//< currently executed command
	proc::ProcessorProvider* m_provider;					//< processor provider

	///\brief Helper function to send a line message with CRLF termination as C string
	///\param [in] str head of the line to write
	///\param [in] arg optional argument string of the line to write
	///\return network operation
	const net::NetworkOperation WriteLine( const char* str, const char* arg=0);

	///\brief Helper function to send a line message with CRLF termination as C string
	///\param [in] str head of the line to write
	///\param [in] arg integer argument of the line to write
	///\return network operation
	const net::NetworkOperation WriteLine( const char* str, int code);

	///\brief Get the next read data operation
	///\return network operation
	const net::NetworkOperation readDataOp();
};
} // namespace iproc


///\brief The server handler container
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

