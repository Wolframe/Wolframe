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
/// \file processor/execContext.hpp
/// \brief Execution context
#ifndef _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#define _WOLFRAME_PROCESSOR_EXEC_CONTEXT_HPP_INCLUDED
#include "processor/procProviderInterface.hpp"
#include "types/secureReference.hpp"
#include "types/keymap.hpp"
#include "AAAA/user.hpp"
#include "AAAA/authorization.hpp"
#include "AAAA/authenticator.hpp"
#include "AAAA/AAAAprovider.hpp"

namespace _Wolframe {
namespace proc {

/// \class ExecContext
/// \brief Execution context passed to functions for referencing resources and to define authorization dependend processing
class ExecContext
{
public:
	/// \brief Default Constructor
	ExecContext()
		:m_provider(0),m_authorizer(0),m_aaaaProvider(0),m_default_timeout(0){}
	/// \brief Constructor
	ExecContext( const ProcessorProviderInterface* p, const AAAA::AAAAprovider* a)
		:m_provider(p),m_authorizer(0),m_aaaaProvider(a),m_default_timeout(0){}

	/// \brief Get the processor provider interface
	const ProcessorProviderInterface* provider() const	{return m_provider;}

	/// \brief Get the the user instance reference
	const AAAA::User* user() const				{return m_user.get();}
	/// \brief Set the user instance
	/// \remark Allocated with new, owned by this from now
	void setUser( AAAA::User* u)				{m_user.reset( u);}

	/// \brief Get the authorization instance interface
	const AAAA::Authorizer* authorizer() const		{return m_authorizer;}
	/// \brief Set the authorization instance interface
	void setAuthorizer( const AAAA::Authorizer* a)		{m_authorizer = a;}

	/// \brief Get the default timeout for read operations in seconds
	unsigned int defaultTimeout() const			{return m_default_timeout;}
	/// \brief Set the default timeout for read operations in seconds (0=forever)
	void setDefaultTimeout( unsigned int timeout_sec_)	{m_default_timeout = timeout_sec_;}

	/// \brief Get the socket identifier for authorization checks
	const char* socketIdentifier() const			{return m_localEndpoint.get()?m_localEndpoint->config().socketIdentifier.c_str():0;}
	/// \brief Get the remote endpoint for authorization checks
	const net::RemoteEndpoint* remoteEndpoint() const	{return m_remoteEndpoint.get();}
	/// \brief Get the local endpoint for authorization checks
	const net::LocalEndpoint* localEndpoint() const		{return m_localEndpoint.get();}

	/// \brief Set the socket identifier for authorization checks
	void setConnectionData(
			const net::RemoteEndpointR& remoteEndpoint_,
			const net::LocalEndpointR& localEndpoint_)
	{
		m_remoteEndpoint = remoteEndpoint_;
		m_localEndpoint = localEndpoint_;
	}

	/// \brief Ask for a capability for this execution context
	bool hasCapability( const std::string& c) const
	{
		if (!m_localEndpoint.get()) return false;
		return m_localEndpoint->config().hasCapability( c);
	}

	/// \brief Get an authenticator
	AAAA::Authenticator* authenticator( const net::RemoteEndpoint& client ) const
	{
		return m_aaaaProvider?m_aaaaProvider->authenticator( client ):0;
	}
	/// \brief Get a password changer
	AAAA::PasswordChanger* passwordChanger( const net::RemoteEndpoint& client ) const
	{
		return m_aaaaProvider?m_aaaaProvider->passwordChanger( *m_user.get(), client ):0;
	}

	/// \brief Checks if a function tagged with AUTHORIZE( funcname, resource) is allowed to be executed
	bool checkAuthorization( const std::string& funcname, const std::string& resource, std::string& errmsg, bool allowIfNotExists=false);

	/// \brief Hardcoded basic authorization function enumeration
	enum BasicAuthorizationFunction
	{
		CONNECT,
		PASSWD
	};
	/// \brief Get the name of a basic function
	static const char* basicAuthorizationFunctionName( BasicAuthorizationFunction n)
	{
		static const char* ar[] = {"CONNECT","PASSWD"};
		return ar[n];
	}

	/// \brief Checks authorization for a basic function
	bool checkAuthorization( BasicAuthorizationFunction f)
	{
		std::string errmsg;
		return checkAuthorization( basicAuthorizationFunctionName(f), "", errmsg, true);
	}

	/// \brief Create a new transaction object
	db::Transaction* transaction( const std::string& name);

	/// \brief Declare the database 'dbname' as the current transaction database
	void push_database( const std::string& dbname)			{m_dbstack.push_back( dbname);}
	/// \brief Restore the previous current transaction database
	void pop_database()						{m_dbstack.pop_back();}

private:
	ExecContext( const ExecContext&);			//... non copyable
	void operator=( const ExecContext&);			//... non copyable

private:
	const ProcessorProviderInterface* m_provider;		///< processor provider interface
	types::SecureReference<AAAA::User> m_user;		///< user instance
	const AAAA::Authorizer* m_authorizer;			///< instance to query for execution permission based on login data
	const AAAA::AAAAprovider* m_aaaaProvider;		///< instance to query for an authenticator
	unsigned int m_default_timeout;				///< default timeout
	net::RemoteEndpointR m_remoteEndpoint;			///< remote end point of the connection
	net::LocalEndpointR m_localEndpoint;			///< local end point of the connection
	std::vector<std::string> m_dbstack;			///< stack for implementing current database as scope
};

}} //namespace
#endif

