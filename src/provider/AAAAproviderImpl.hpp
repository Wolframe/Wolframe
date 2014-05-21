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
//
// AAAA provider implementation
//

#ifndef _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
#define _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED

#include "AAAA/AAAAprovider.hpp"
#include "config/configurationBase.hpp"
#include "AAAA/authUnit.hpp"
#include "AAAA/authorization.hpp"
#include "AAAA/audit.hpp"

#include "database/DBprovider.hpp"

#include <string>
#include <list>
#include <vector>

namespace _Wolframe {
namespace AAAA {

// Standard authentication class and authentication provider
class StandardAuthenticator : public Authenticator
{
public:
	StandardAuthenticator( const std::vector<std::string>& mechs_ );
	~StandardAuthenticator();
	void close();

	/// Get the list of available mechs
	virtual const std::vector<std::string>& mechs() const;

	/// Set the authentication mech
	virtual bool setMech( const std::string& mech );

	/// Input message
	virtual void messageIn( const void* message, std::size_t size );

	/// Output message
	virtual int messageOut( const void** message, std::size_t size );

	/// The current status of the authenticator
	virtual Status status() const;

	/// The authenticated user or NULL if not authenticated
	virtual User* user() const;
private:
	const std::vector<std::string>&	m_mechs;
};

class AuthenticationFactory
{
public:
	AuthenticationFactory( const std::list< config::NamedConfiguration* >& confs,
			       const module::ModulesDirectory* modules );
	~AuthenticationFactory();
	bool resolveDB( const db::DatabaseProvider& db );

	Authenticator* authenticator();
private:
	std::list< AuthenticationUnit* > m_authenticators;
	std::vector< std::string >	m_mechs;
};


// Standard authorization classes and authorization provider
class StandardAuthorizer : public Authorizer
{
public:
	StandardAuthorizer( const std::list< AuthorizationUnit* >& units, bool dflt );
	~StandardAuthorizer();
	void close();

	bool allowed( const Information& authzObject );
private:
	const std::list< AuthorizationUnit* >&	m_authorizeUnits;
	bool m_default;
};

class AuthorizationProvider
{
public:
	AuthorizationProvider( const std::list< config::NamedConfiguration* >& confs,
			       bool authzDefault,
			       const module::ModulesDirectory* modules );
	~AuthorizationProvider();
	bool resolveDB( const db::DatabaseProvider& db );

	Authorizer* authorizer() const		{ return m_authorizer; }
private:
	std::list< AuthorizationUnit* >	m_authorizeUnits;
	StandardAuthorizer*		m_authorizer;
};


// Standard audit class and audit provider
class StandardAudit : public Auditor
{
public:
	StandardAudit( const std::list< AuditUnit* >& units, bool mandatory );
	~StandardAudit();
	void close();

	bool audit( const Information& );
private:
	const std::list< AuditUnit* >&	m_auditUnits;
	bool				m_mandatory;
};

class AuditProvider
{
public:
	AuditProvider( const std::list< config::NamedConfiguration* >& confs,
		       const module::ModulesDirectory* modules );
	~AuditProvider();
	bool resolveDB( const db::DatabaseProvider& db );

	Auditor* auditor()			{ return m_auditor; }
private:
	std::list< AuditUnit* >		m_auditors;
	StandardAudit*			m_auditor;
};


// AAAA provider PIMPL class
class AAAAprovider::AAAAprovider_Impl
{
public:
	AAAAprovider_Impl( const AAAAconfiguration* conf,
			   const module::ModulesDirectory* modules );
	~AAAAprovider_Impl()				{}
	bool resolveDB( const db::DatabaseProvider& db );

	Authenticator* authenticator()		{ return m_authenticator.authenticator(); }
	Authorizer* authorizer()		{ return m_authorizer.authorizer(); }
	Auditor* auditor()			{ return m_auditor.auditor(); }
private:
	AuthenticationFactory	m_authenticator;
	AuthorizationProvider	m_authorizer;
	AuditProvider		m_auditor;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_IMPLEMENTATION_HPP_INCLUDED
