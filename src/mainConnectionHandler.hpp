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
///\file mainConnectionHandler.hpp
///
#ifndef _Wolframe_MAIN_CONNECTION_HANDLER_HPP_INCLUDED
#define _Wolframe_MAIN_CONNECTION_HANDLER_HPP_INCLUDED
#include "system/connectionHandler.hpp"
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/lineCommandHandler.hpp"
#include "cmdbind/doctypeFilterCommandHandler.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include "mainConnectionHandler_auth.hpp"
#include "protocol/ioblocks.hpp"
#include "types/keymap.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {

class CommandHandler :public cmdbind::LineCommandHandlerTemplate<CommandHandler>
{
public:
	typedef cmdbind::LineCommandHandlerTemplate<CommandHandler> Parent;
	CommandHandler();
	virtual ~CommandHandler(){}

public:
	int doAuth( int argc, const char** argv, std::ostream& out);

	int doMech( int argc, const char** argv, std::ostream& out);
	int endMech( cmdbind::CommandHandler* ch, std::ostream& out);

	int doRequest( int argc, const char** argv, std::ostream& out);
	int endRequest( cmdbind::CommandHandler* ch, std::ostream& out);

	int doInterface( int argc, const char** argv, std::ostream& out);
	int endInterface( cmdbind::CommandHandler* ch, std::ostream& out);

	int doCapabilities( int argc, const char** argv, std::ostream& out);
	int doQuit( int argc, const char** argv, std::ostream& out);

	int endDoctypeDetection( cmdbind::CommandHandler* ch, std::ostream& out);
	int endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out);

private:
	bool redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out);
	std::list<std::string> roles() const
	{
		std::list<std::string> rt;
		rt.push_back( "std");		//TODO: To be extracted from m_authtickets
		return rt;
	}

private:
	AuthMechanisms m_authMechanisms;
	std::string m_command;
	std::string m_commandtag;
	std::vector<std::string> m_authtickets;
};


/// The connection handler
class Connection : public net::ConnectionHandler
{
public:
	///\brief Constructor
	Connection( const net::LocalEndpoint& local);

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

	///\brief Set the reference to the execution context
	void setExecContext( proc::ExecContext* context_)
	{
		m_cmdhandler.setExecContext( context_);
	}

private:
	CommandHandler m_cmdhandler;			//< top level instance executing commands
	protocol::InputBlock m_input;			//< buffer for network read messages
	protocol::OutputBlock m_output;			//< buffer for network write messages
	bool m_terminated;				//< true, if a termination signal came from the network
};
} // namespace proc
} // namespace _Wolframe
#endif

