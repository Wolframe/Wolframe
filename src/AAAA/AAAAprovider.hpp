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
// AAAA provider
//

#ifndef _AAAA_PROVIDER_HPP_INCLUDED
#define _AAAA_PROVIDER_HPP_INCLUDED

#include "configurationBase.hpp"
#include "auditor.hpp"
#include "database.hpp"

#include <string>
#include <list>

namespace _Wolframe {
namespace AAAA {

class User
{
public:
	User();
};


/// Authentication type
enum AuthenticationType	{
	AUTH_WOLFRAME,		///< Wolframe proprietary authentification
	AUTH_PAM,		///< *NIX PAM authentification
	AUTH_SASL,		///< SASL authentification (Cyrus SASL)
	AUTH_LDAP,		///< LDAP authentification
	AUTH_UNKNOWN		///< Unknown authentification (i.e. none)
};


class AuthenticationConfigBase : public config::ConfigurationBase
{
private:
	const AuthenticationType	m_type;
public:
	/// constructor
	AuthenticationConfigBase( const AuthenticationType Type,
				  const char* name, const char* logParent, const char* logName )
		: config::ConfigurationBase( name, logParent, logName ),
		  m_type( Type )
	{}

	AuthenticationType type() const		{ return m_type; }
};


class AuthenticationConfiguration : public config::ConfigurationBase
{
	friend class AAAAprovider;
public:
	/// constructor
	AuthenticationConfiguration( const char* cfgName, const char* logParent, const char* logName );

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	std::list<AuthenticationConfigBase*>	m_config;
};

class Authenticator
{
public:
	//		Authenticator( AuthenticatorConfiguration& conf );
};


struct Configuration : public config::ConfigurationBase
{
	friend class AAAAprovider;
public:
	/// constructor
	Configuration();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	AuthenticationConfiguration	auth;
	AuditConfiguration		audit;
};


class AAAAprovider
{
public:
	AAAAprovider( const Configuration& conf );
	~AAAAprovider();
	bool resolveDB( db::DBprovider& db );
private:
	std::list<Authenticator*>	m_authenticators;
	std::list<Auditor*>		m_auditors;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED
