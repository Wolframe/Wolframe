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

enum AuditType	{
	AUDIT_FILE,		///< Standard text file auditing (list of entries)
	AUDIT_DATABASE,		///< Database audit entries
	AUDIT_UNKNOWN		///< Unknown method (i.e. none)
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

	//	AuthenticationConfigBase();

	AuthenticationType type() const		{ return m_type; }
};

class AuditConfigBase : public config::ConfigurationBase
{
private:
	const AuditType	m_type;
public:
	/// constructor
	AuditConfigBase( const AuditType Type,
			 const char* name, const char* logParent, const char* logName )
		: config::ConfigurationBase( name, logParent, logName ),
		  m_type( Type )
	{}

	//	AuditConfigBase();

	AuditType type() const			{ return m_type; }
};


class AuthenticationConfiguration : public config::ConfigurationBase
{
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
	std::list<AuthenticationConfigBase*>	m_authConfig;
};

class Authenticator
{
public:
	//		Authenticator( AuthenticatorConfiguration& conf );
};


class Authorizer
{
public:
};


class FileAuditConfig : public AuditConfigBase
{
public:
	FileAuditConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuditConfigBase( AUDIT_FILE, cfgName, logParent, logName )
	{}
	// ~FileAuditConfig();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

private:
	std::string	file;
};


class DatabaseAuditConfig : public AuditConfigBase
{
public:
	DatabaseAuditConfig( const char* cfgName, const char* logParent, const char* logName )
		: AuditConfigBase( AUDIT_FILE, cfgName, logParent, logName )
	{}
	// ~DatabaseAuditConfig();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

private:
};


class AuditConfiguration : public config::ConfigurationBase
{
public:
	/// constructor
	AuditConfiguration( const char* cfgName, const char* logParent, const char* logName );

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
private:
	std::list<AuditConfigBase*>	m_auditConfig;
};

class Auditor
{
public:
//	Auditor( AuditorConfiguration& conf );
};


struct Configuration : public config::ConfigurationBase
{
	AuthenticationConfiguration	auth;
	AuditConfiguration		audit;
public:
	/// constructor
	Configuration();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	// bool test() const;	// Not implemented yet, inherited from base
};

class AAAAprovider
{
public:
	AAAAprovider( const Configuration& conf );

	const Authenticator& authenticator()	{ return authenticator_; }
	const Authorizer& authorizer()		{ return authorizer_; }
	const Auditor& auditor()		{ return auditor_; }
private:
	const Authenticator	authenticator_;
	const Authorizer	authorizer_;
	const Auditor		auditor_;
};

}} // namespace _Wolframe::AAAA

#endif // _AAAA_PROVIDER_HPP_INCLUDED
