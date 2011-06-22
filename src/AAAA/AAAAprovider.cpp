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
// AAAA provider implementation
//

#include <stdexcept>

#include "AAAAprovider.hpp"
#include "logger.hpp"

#include "FileAudit.hpp"
#include "DBaudit.hpp"
#include "TextFileAuthentication.hpp"
#include "DBauthentication.hpp"
#include "boost/algorithm/string.hpp"

namespace _Wolframe {
namespace AAAA {

AAAAprovider::AAAAprovider( const AAAAconfiguration& conf )
{
	for ( std::list<AuthenticatorConfigBase*>::const_iterator it = conf.auth.m_config.begin();
							it != conf.auth.m_config.end(); it++ )	{
		const char* type = (*it)->typeName();
		if ( boost::algorithm::iequals( type, "DatabaseAuth" ))	{
			DBauthContainer* auth = new DBauthContainer( static_cast<DatabaseAuthConfig&>(**it) );
			m_authenticators.push_back( auth );
		}
		else if ( boost::algorithm::iequals( type, "TextFileAuth" ))	{
			TxtFileAuthContainer* auth = new TxtFileAuthContainer( static_cast<TextFileAuthConfig&>(**it) );
			m_authenticators.push_back( auth );
		}
		else
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor" );
	}

	for ( std::list<AuditConfigurationBase*>::const_iterator it = conf.audit.m_config.begin();
							it != conf.audit.m_config.end(); it++ )	{
		const char* type = (*it)->typeName();

		if ( boost::algorithm::iequals( type, "FileAudit" ))	{
			FileAuditor* auditor = new FileAuditor( static_cast<FileAuditConfig&>(**it) );
			m_auditors.push_back( auditor );
		}
		if ( boost::algorithm::iequals( type, "DatabaseAudit" ))	{
			DBauditContainer* auditor = new DBauditContainer( static_cast<DatabaseAuditConfig&>(**it) );
			m_auditors.push_back( auditor );
		}
		else
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor" );
	}
}

AAAAprovider::~AAAAprovider()
{
	for ( std::list<AuthenticatorBase*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
	for ( std::list<AuditorBase*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		delete *it;
}

bool AAAAprovider::resolveDB( db::DatabaseProvider& db )
{
	for ( std::list<AuthenticatorBase*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;

	for ( std::list<AuditorBase*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


AAAAconfiguration::AAAAconfiguration() : config::ConfigurationBase( "AAAA", NULL, "AAAA configuration"  ),
	auth( "Authentication", logPrefix().c_str(), "Authentication" ),
	audit( "Auditing", logPrefix().c_str(), "Auditing" )
{
}


/// methods
void AAAAconfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	auth.print( os, 3 );
	audit.print( os, 3 );
}


/// Check if the database configuration makes sense
bool AAAAconfiguration::check() const
{
	bool correct = true;

	if ( !auth.check() )
		correct = false;
	if ( !audit.check() )
		correct = false;

	return correct;
}

void AAAAconfiguration::setCanonicalPathes( const std::string& refPath )
{
	auth.setCanonicalPathes( refPath );
	audit.setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::AAAA

