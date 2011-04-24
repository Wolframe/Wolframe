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
/// \file iprocHandler.hpp
/// \brief simple method table processor connection handler
///

#ifndef _Wolframe_PROTOCOL_iproc_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_iproc_HANDLER_HPP_INCLUDED
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"

namespace _Wolframe {
namespace iproc {

	/// The connection handler
	class Connection : public net::connectionHandler
	{
	public:
		typedef net::NetworkOperation Operation;

		Connection( const net::LocalEndpoint& local, const AppConfiguration& config);

		virtual ~Connection();

		virtual void setPeer( const net::RemoteEndpoint& remote);

		/// Handle a request and produce a reply.
		virtual const Operation nextOperation();
		virtual void networkInput( const void *begin, std::size_t bytesTransferred);

		virtual void timeoutOccured();
		virtual void signalOccured();
		virtual void errorOccured( NetworkSignal);
	public:
		struct Private;
	private:
		Private* data;
	};

} // namespace iproc

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		net::connectionHandler* newConnection( const net::LocalEndpoint& local);

	};

} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_iproc_HANDLER_HPP_INCLUDED
