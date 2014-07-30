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
///\file standardProtocolHandler.hpp

#ifndef _Wolframe_STANDARD_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_STANDARD_PROTOCOL_HANDLER_HPP_INCLUDED
#include "cmdbind/protocolHandler.hpp"
#include "lineProtocolHandler.hpp"
#include "doctypeFilterProtocolHandler.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace cmdbind {

class StandardProtocolHandler
	:public LineProtocolHandlerTemplate<StandardProtocolHandler>
{
public:
	typedef cmdbind::LineProtocolHandlerTemplate<StandardProtocolHandler> Parent;

	StandardProtocolHandler();
	virtual ~StandardProtocolHandler(){}

	virtual void setPeer( const net::RemoteEndpointR& remote);
	virtual void setLocalEndPoint( const net::LocalEndpointR& local);
	virtual void setOutputBuffer( void* buf, std::size_t size, std::size_t pos);

public:
	int doAuth( int argc, const char** argv, std::ostream& out);

	int doPasswordChange( int argc, const char** argv, std::ostream& out);
	int endPasswordChange( cmdbind::ProtocolHandler* ch, std::ostream& out);

	int doMech( int argc, const char** argv, std::ostream& out);
	int endMech( cmdbind::ProtocolHandler* ch, std::ostream& out);

	int doRequest( int argc, const char** argv, std::ostream& out);
	int endRequest( cmdbind::ProtocolHandler* ch, std::ostream& out);

	int doInterface( int argc, const char** argv, std::ostream& out);
	int endInterface( cmdbind::ProtocolHandler* ch, std::ostream& out);

	int doCapabilities( int argc, const char** argv, std::ostream& out);
	int doQuit( int argc, const char** argv, std::ostream& out);

	int endDoctypeDetection( cmdbind::ProtocolHandler* ch, std::ostream& out);
	int endErrDocumentType( cmdbind::ProtocolHandler* ch, std::ostream& out);

private:
	bool redirectConsumedInput( cmdbind::DoctypeFilterProtocolHandler* fromh, cmdbind::ProtocolHandler* toh, std::ostream& out);

private:
	boost::shared_ptr<AAAA::Authenticator> m_authenticator;
	boost::shared_ptr<AAAA::PasswordChanger> m_passwordChanger;
	net::RemoteEndpointR m_remoteEndpoint;
	net::LocalEndpointR m_localEndpoint;
	std::string m_command;
	std::string m_commandtag;
	std::size_t m_outputChunkSize;
};


/// \class StandardProtocolHandlerUnit
/// \brief Class that defines the Wolframe standard protocol handler 
class StandardProtocolHandlerUnit
	:public ProtocolHandlerUnit
{
public:
	/// \brief Get the name of the Wolframe standard protocol handler
	virtual const char* protocol() const
	{
		return "wolframe";
	}

	/// \brief Create an instance of the Wolframe standard protocol handler
	virtual ProtocolHandler* createProtocolHandler()
	{
		return new StandardProtocolHandler();
	}
};

}}//namespace
#endif

