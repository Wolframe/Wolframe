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
// PostgreSQL configuration parser
//

#include "PostgreSQL.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "utils/fileUtils.hpp"
#include "serialize/struct/structDescription.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

const serialize::StructDescriptionBase* PostgreSQLConfig::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<PostgreSQLConfig>
	{
	ThisDescription()
	{
		(*this)
		( "identifier", &PostgreSQLConfig::m_ID)		.mandatory()
		( "host", &PostgreSQLConfig::m_host )			.optional()
		( "port", &PostgreSQLConfig::m_port )			.optional()
		( "database", &PostgreSQLConfig::m_dbName )		.optional()
		( "user", &PostgreSQLConfig::m_user )			.optional()
		( "password", &PostgreSQLConfig::m_password )		.optional()
		( "m_sslMode", &PostgreSQLConfig::m_sslMode )		.optional()
		( "sslCert", &PostgreSQLConfig::m_sslCert )		.optional()
		( "sslKey", &PostgreSQLConfig::m_sslKey )		.optional()
		( "sslRootCert", &PostgreSQLConfig::m_sslRootCert )	.optional()
		( "sslCRL", &PostgreSQLConfig::m_sslCRL )		.optional()
		( "connectionTimeout", &PostgreSQLConfig::m_connectTimeout ).optional()
		( "connections", &PostgreSQLConfig::m_connections )	.optional()
		( "acquireTimeout", &PostgreSQLConfig::m_acquireTimeout ).optional()
		( "statementTimeout", &PostgreSQLConfig::m_statementTimeout ).optional()
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}

bool PostgreSQLConfig::mapValueDomains()
{
	bool retVal = true;
	if (m_port == 0)
	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": port must be defined as a non zero non negative number";
		retVal = false;
	}
	if (!m_sslMode.empty())
	{
		if ( boost::algorithm::iequals( m_sslMode, "disable" ))
			m_sslMode = "disable";
		else if ( boost::algorithm::iequals( m_sslMode, "allow" ))
			m_sslMode = "allow";
		else if ( boost::algorithm::iequals( m_sslMode, "prefer" ))
			m_sslMode = "prefer";
		else if ( boost::algorithm::iequals( m_sslMode, "require" ))
			m_sslMode = "require";
		else if ( boost::algorithm::iequals( m_sslMode, "verify-ca" ))
			m_sslMode = "verify-ca";
		else if ( boost::algorithm::iequals( m_sslMode, "verify-full" ))
			m_sslMode = "verify-full";
		else	{
			LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": unknown SSL mode: '" << m_sslMode << "'";
			retVal = false;
		}
	}
	if ( !m_sslCert.empty() && m_sslKey.empty() )	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": SSL certificate configured but no SSL key specified";
		retVal = false;
	}
	if ( !m_sslCert.empty() && m_sslKey.empty() )	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": SSL key configured but no SSL certificate specified";
		retVal = false;
	}
	if ( boost::algorithm::iequals( m_sslMode, "verify-ca" ) ||
	     boost::algorithm::iequals( m_sslMode, "verify-full" ))	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": server SSL certificate requested but no root CA specified";
		retVal = false;
	}
	if ( m_sslMode.empty())
		m_sslMode = "prefer";
	return retVal;
}

bool PostgreSQLConfig::parse( const config::ConfigurationNode& pt, const std::string& node,
			      const module::ModulesDirectory* modules )
{
	bool rt;
	rt = _Wolframe::serialize::DescriptiveConfiguration::parse( pt, node, modules);
	if( rt ) {
		rt = mapValueDomains( );
	}
	return rt;
}

void PostgreSQLConfig::setCanonicalPathes( const std::string& refPath )
{
	if ( ! m_sslCert.empty() )	{
		std::string oldPath = m_sslCert;
		m_sslCert = utils::getCanonicalPath( m_sslCert, refPath);
		if ( oldPath != m_sslCert ) {
			LOG_WARNING << logPrefix() << "Using absolute SSL certificate filename '" << m_sslCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_sslKey.empty() )	{
		std::string oldPath = m_sslKey;
		m_sslKey = utils::getCanonicalPath( m_sslKey, refPath );
		if ( oldPath != m_sslKey ) {
			LOG_WARNING << logPrefix() << "Using absolute SSL key filename '" << m_sslKey
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_sslRootCert.empty() )	{
		std::string oldPath = m_sslRootCert;
		m_sslRootCert = utils::getCanonicalPath( m_sslRootCert, refPath );
		if ( oldPath != m_sslRootCert ) {
			LOG_WARNING << logPrefix() << "Using absolute CA certificate filename '" << m_sslRootCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! m_sslCRL.empty() )	{
		std::string oldPath = m_sslCRL;
		m_sslCRL = utils::getCanonicalPath( m_sslCRL, refPath );
		if ( oldPath != m_sslCRL ) {
			LOG_WARNING << logPrefix() << "Using absolute CRL filename '" << m_sslCRL
				       << "' instead of '" << oldPath << "'";
		}
	}
}

void PostgreSQLConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	if ( m_host.empty())
		os << indStr << "   Database host: local unix domain socket" << std::endl;
	else
		os << indStr << "   Database host: " << m_host << ":" << m_port << std::endl;
	os << indStr << "   Database name: " << (m_dbName.empty() ? "(not specified - server user default)" : m_dbName) << std::endl;
	os << indStr << "   Database user: " << (m_user.empty() ? "(not specified - same as server user)" : m_user)
	   << ", password: " << (m_password.empty() ? "(not specified - no password used)" : m_password) << std::endl;
	if ( ! m_sslMode.empty())
		os << indStr << "   Database connection SSL mode: " << m_sslMode << std::endl;
	if ( ! m_sslCert.empty())	{
		os << indStr << "   Client SSL certificate file: " << m_sslCert << std::endl;
		if ( ! m_sslMode.empty())
			os << indStr << "   Client SSL key file: " << m_sslKey << std::endl;
	}
	if ( ! m_sslRootCert.empty())
		os << indStr << "   SSL root CA file: " << m_sslRootCert << std::endl;
	if ( ! m_sslCRL.empty())
		os << indStr << "   SSL CRL file: " << m_sslCRL << std::endl;

	if ( m_connectTimeout == 0 )
		os << indStr << "   Connect timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Connect timeout: " << m_connectTimeout << "s" << std::endl;
	os << indStr << "   Database connections: " << m_connections << std::endl;
	if ( m_acquireTimeout == 0 )
		os << indStr << "   Acquire database connection timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Acquire database connection timeout: " << m_acquireTimeout << "s" << std::endl;
	if ( m_statementTimeout == 0 )
		os << indStr << "   Default statement execution timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Default statement execution timeout: " << m_statementTimeout << "ms" << std::endl;
}

bool PostgreSQLConfig::check() const
{
	if ( m_connections == 0 )	{
		LOG_ERROR << logPrefix() << " " << m_config_pos.logtext() << ": number of database connections cannot be 0";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config
