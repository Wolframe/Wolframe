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
/// \file protocol/protocolHandler.hpp
/// \brief Interface to protocol handler
#ifndef _Wolframe_CMDBIND_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_CMDBIND_PROTOCOL_HANDLER_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "system/connectionEndpoint.hpp"
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

/// \class ProtocolHandler
/// \brief Protocol handler interface
class ProtocolHandler
	:public cmdbind::CommandHandler
{
public:
	/// \brief Operation type
	enum Operation	{
		READ,			/// <request to read data
		WRITE,			/// <request to write data
		CLOSE			/// <request to terminate processing
	};

	/// \brief Destructor
	virtual ~ProtocolHandler(){}

	/// \brief Set the client connection end point
	/// \param [in] remote the end point to set
	virtual void setPeer( const net::RemoteEndpoint& remote)=0;

	/// \brief Set the local connection end point
	/// \param [in] local the local point to set
	virtual void setLocalEndPoint( const net::LocalEndpoint& local)=0;
};

typedef boost::shared_ptr<ProtocolHandler> ProtocolHandlerR;


/// \class ProtocolHandlerUnit
/// \brief Class that defines a protocol handler class and is able to create instances of it
class ProtocolHandlerUnit
{
public:
	/// \brief Get the name of the protocol implemented
	virtual const char* protocol() const=0;

	/// \brief Create an instance of this protocol handler
	virtual ProtocolHandler* createProtocolHandler()=0;
};

/// \brief Protocol handler unit reference
typedef boost::shared_ptr<ProtocolHandlerUnit> ProtocolHandlerUnitR;

}}
#endif

