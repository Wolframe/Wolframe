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
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <string>
#include <cstddef>
#include <cstring>

#include "connectionEndpoint.hpp"

namespace _Wolframe {
	namespace net {

		/// Base class for a network operation. It should never be accessed directly by
		/// the user code
		class	NetworkOperation
		{
			template< typename T > friend class connectionBase;

		protected:
			enum Operation	{
				READ,
				WRITE,
				CLOSE
			};

			explicit NetworkOperation( Operation op, void* d = NULL, std::size_t s = 0,
						   unsigned to = 0 )
						{ operation_ = op,  data_ = d; size_ = s; timeout_ = to; }

			unsigned timeout()	{ return timeout_; }
			const void* data()	{ return (const void*)data_; }
			void* buffer()		{ return data_; }
			std::size_t size()	{ return size_; }
			Operation operation()	{ return operation_; }

		private:
			Operation	operation_;
			unsigned	timeout_;
			void*		data_;
			std::size_t	size_;
		};

		/// Network operation: asynchronously read a block of data
		class ReadData : public NetworkOperation
		{
		public:
			explicit ReadData( void* d, std::size_t s, unsigned to = 0 )
				: NetworkOperation( READ, d, s, to )	{}
		};

		/// Network operation: asynchronously send a block of data
		class SendData : public NetworkOperation
		{
		public:
			explicit SendData( const void* d, std::size_t s, unsigned to = 0 )
				: NetworkOperation( WRITE, const_cast<void*>( d ), s, to )	{}
		};

		/// Network operation: asynchronously send a string (message)
		/// This is just some syntactic sugar, simplifying SendData
		class SendString : public NetworkOperation
		{
		public:
			explicit SendString( const std::string& s, unsigned to = 0 )
				: NetworkOperation( WRITE, const_cast<char*>( s.c_str() ), s.length(), to )	{}
			explicit SendString( const char *s, unsigned to = 0 )
				: NetworkOperation( WRITE, const_cast<char*>( s ), strlen( s ), to )	{}
		};

		/// Network operation: close the current network connection
		class CloseConnection : public NetworkOperation
		{
		public:
			CloseConnection() : NetworkOperation( CLOSE )	{}
		};


		/// The common handler for the connection status.
		class connectionHandler
		{
			template< typename socketType > friend class connectionBase;
		protected:
			connectionHandler()		{}
			virtual ~connectionHandler()	{}

		private:
			connectionHandler( const connectionHandler& );
			connectionHandler& operator = ( const connectionHandler& );

		public:
			enum NetworkSignal	{
				END_OF_FILE,
				OPERATION_CANCELLED,
				BROKEN_PIPE,
				UNKNOWN_ERROR
			};


			/// Signal the incoming data. buffer is the buffer given to the read operation
			virtual void networkInput( const void* buffer, std::size_t bytesTransferred ) = 0;

			/// What should the network do next.
			virtual const NetworkOperation nextOperation() = 0;

			/// Timeout timer was fired.
			virtual void timeoutOccured()			{}

			/// A signal was received from outside.
			virtual void signalOccured()			{}

			/// An error network occured
			virtual void errorOccured( NetworkSignal )	{}

			/// Set the remote peer. The connection is up now.
			virtual void setPeer( const RemoteEndpoint& remote ) = 0;
		};

	} // namespace net

	/// The server main
	/// All it should do is to provide connection handlers

	struct	HandlerConfiguration;

	class ServerHandler
	{
	public:
		ServerHandler( const HandlerConfiguration *config );
		~ServerHandler();

		/// Create a new connection handler and return a pointer to it
		net::connectionHandler* newConnection( const net::LocalEndpoint& local );

	private:
		ServerHandler( const ServerHandler& );
		ServerHandler& operator = ( const ServerHandler& );

		class ServerHandlerImpl;
		ServerHandlerImpl *impl_;
	};

} // namespace _Wolframe

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
