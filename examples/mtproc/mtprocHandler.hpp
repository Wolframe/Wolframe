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
/// \file mtprocHandler.hpp
/// \brief simple method table processor connection handler
///

#ifndef _Wolframe_PROTOCOL_MTPROC_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_MTPROC_HANDLER_HPP_INCLUDED
#include "connectionHandler.hpp"
#include "methodtable.hpp"


namespace _Wolframe {
namespace mtproc {

	/// The connection handler
	class Connection : public Network::connectionHandler
	{
	public:
		typedef Network::NetworkOperation Operation;

		Connection( const Network::LocalEndpoint& local, unsigned int inputBufferSize=128, unsigned int outputBufferSize=128);

		virtual ~Connection();

		virtual void setPeer( const Network::RemoteEndpoint& remote);

		/// Handle a request and produce a reply.
		virtual const Operation nextOperation();
		virtual void networkInput( const void *begin, std::size_t bytesTransferred);

		virtual void timeoutOccured();
		virtual void signalOccured();
		virtual void errorOccured( NetworkSignal);

		//define the methods active with their context object
		void initObject( Instance* instance);
	public:
		struct Private;
	private:
		Private* data;
	};

} // namespace mtproc

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		Network::connectionHandler* newConnection( const Network::LocalEndpoint& local);

	};

} // namespace _Wolframe

#endif // _Wolframe_PROTOCOL_MTPROC_HANDLER_HPP_INCLUDED
