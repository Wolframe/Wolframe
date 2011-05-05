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
//
// wolframeHandler.hpp - wolframe main handler
//

#ifndef _Wolframe_HANDLER_HPP_INCLUDED
#define _Wolframe_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"
#include "database.hpp"
#include "AAAA/AAAAprovider.hpp"

namespace _Wolframe {
	/// The global server container
	class wolframeHandler
	{
	public:
		wolframeHandler( const HandlerConfiguration* config );
		~wolframeHandler();

		const std::string& banner() const	{ return m_banner; }
		const db::DBprovider db() const		{ return m_db; }
		const AAAA::AAAAprovider& aaaa()	{ return m_aaaa; }
	private:
		const std::string		m_banner;
		_Wolframe::db::DBprovider	m_db;
		_Wolframe::AAAA::AAAAprovider	m_aaaa;
	};


	/// The connection handler
	class wolframeConnection : public net::connectionHandler
	{
	public:
		wolframeConnection( const wolframeHandler& context, const net::LocalEndpoint& local );
		~wolframeConnection();

		void setPeer( const net::RemoteEndpoint& remote );

		/// Parse / get the incoming data.
		void networkInput( const void *begin, std::size_t bytesTransferred );

		void timeoutOccured();
		void signalOccured();
		void errorOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const net::NetworkOperation nextOperation();

	private:
		enum State	{
			NEW,
			HELLO_SENT,
			READ_INPUT,
			OUTPUT_MSG,
			TIMEOUT,
			SIGNALLED,
			TERMINATE,
			FINISHED
		};
		/// Back link to global context
		const wolframeHandler&		globalCtx_;

		/// Connection endpoints
		const net::LocalEndpoint*	localEP_;
		const net::RemoteEndpoint*	remoteEP_;

		static const std::size_t ReadBufSize = 8192;
		/// The state of the processor FSM
		State		state_;
		/// Read buffer
		char		readBuf_[ ReadBufSize ];
		char*		dataStart_;
		std::size_t	dataSize_;
		/// Output buffer
		std::string	outMsg_;
		/// Idle timeout value
		unsigned	idleTimeout_;
	};

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		ServerHandlerImpl( const HandlerConfiguration *config );
		~ServerHandlerImpl();
		net::connectionHandler* newConnection( const net::LocalEndpoint& local );
	private:
		wolframeHandler	globalContext_;
	};

} // namespace _Wolframe

#endif // _Wolframe_HANDLER_HPP_INCLUDED
