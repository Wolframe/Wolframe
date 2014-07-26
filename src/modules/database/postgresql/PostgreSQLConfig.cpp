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
		( "sslMode", &PostgreSQLConfig::sslMode )		.optional()
		( "sslCert", &PostgreSQLConfig::sslCert )		.optional()
		( "sslKey", &PostgreSQLConfig::sslKey )			.optional()
		( "sslRootCert", &PostgreSQLConfig::sslRootCert	)	.optional()
		( "sslCRL", &PostgreSQLConfig::sslCRL )			.optional()
		( "connectionTimeout", &PostgreSQLConfig::connectTimeout ).optional()
		( "connections", &PostgreSQLConfig::connections )	.optional()
		( "acquireTimeout", &PostgreSQLConfig::acquireTimeout )	.optional()
		( "statementTimeout", &PostgreSQLConfig::statementTimeout ).optional()
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
	if (!sslMode.empty())
	{
		if ( boost::algorithm::iequals( sslMode, "disable" ))
			sslMode = "disable";
		else if ( boost::algorithm::iequals( sslMode, "allow" ))
			sslMode = "allow";
		else if ( boost::algorithm::iequals( sslMode, "prefer" ))
			sslMode = "prefer";
		else if ( boost::algorithm::iequals( sslMode, "require" ))
			sslMode = "require";
		else if ( boost::algorithm::iequals( sslMode, "verify-ca" ))
			sslMode = "verify-ca";
		else if ( boost::algorithm::iequals( sslMode, "verify-full" ))
			sslMode = "verify-full";
		else	{
			LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": unknown SSL mode: '" << sslMode << "'";
			retVal = false;
		}
	}
	if ( !sslCert.empty() && sslKey.empty() )	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": SSL certificate configured but no SSL key specified";
		retVal = false;
	}
	if ( !sslCert.empty() && sslKey.empty() )	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": SSL key configured but no SSL certificate specified";
		retVal = false;
	}
	if ( boost::algorithm::iequals( sslMode, "verify-ca" ) ||
	     boost::algorithm::iequals( sslMode, "verify-full" ))	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": server SSL certificate requested but no root CA specified";
		retVal = false;
	}
	if ( sslMode.empty())
		sslMode = "prefer";
	return retVal;
}

bool PostgreSQLConfig::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			      const module::ModulesDirectory* /*modules*/ )
{
	try
	{
		serialize::parseConfigStructure( *static_cast<PostgreSQLConfigStruct*>(this), pt);
		m_config_pos = pt.position();
		return mapValueDomains();
	}
	catch (const std::runtime_error& e)
	{
		LOG_FATAL << logPrefix() << e.what();
		return false;
	}
}

void PostgreSQLConfig::setCanonicalPathes( const std::string& refPath )
{
	if ( ! sslCert.empty() )	{
		std::string oldPath = sslCert;
		sslCert = utils::getCanonicalPath( sslCert, refPath);
		if ( oldPath != sslCert ) {
			LOG_WARNING << logPrefix() << "Using absolute SSL certificate filename '" << sslCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslKey.empty() )	{
		std::string oldPath = sslKey;
		sslKey = utils::getCanonicalPath( sslKey, refPath );
		if ( oldPath != sslKey ) {
			LOG_WARNING << logPrefix() << "Using absolute SSL key filename '" << sslKey
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslRootCert.empty() )	{
		std::string oldPath = sslRootCert;
		sslRootCert = utils::getCanonicalPath( sslRootCert, refPath );
		if ( oldPath != sslRootCert ) {
			LOG_WARNING << logPrefix() << "Using absolute CA certificate filename '" << sslRootCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslCRL.empty() )	{
		std::string oldPath = sslCRL;
		sslCRL = utils::getCanonicalPath( sslCRL, refPath );
		if ( oldPath != sslCRL ) {
			LOG_WARNING << logPrefix() << "Using absolute CRL filename '" << sslCRL
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
	if ( ! sslMode.empty())
		os << indStr << "   Database connection SSL mode: " << sslMode << std::endl;
	if ( ! sslCert.empty())	{
		os << indStr << "   Client SSL certificate file: " << sslCert << std::endl;
		if ( ! sslMode.empty())
			os << indStr << "   Client SSL key file: " << sslKey << std::endl;
	}
	if ( ! sslRootCert.empty())
		os << indStr << "   SSL root CA file: " << sslRootCert << std::endl;
	if ( ! sslCRL.empty())
		os << indStr << "   SSL CRL file: " << sslCRL << std::endl;

	if ( connectTimeout == 0 )
		os << indStr << "   Connect timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Connect timeout: " << connectTimeout << "s" << std::endl;
	os << indStr << "   Database connections: " << connections << std::endl;
	if ( acquireTimeout == 0 )
		os << indStr << "   Acquire database connection timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Acquire database connection timeout: " << acquireTimeout << "s" << std::endl;
	if ( statementTimeout == 0 )
		os << indStr << "   Default statement execution timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Default statement execution timeout: " << statementTimeout << "ms" << std::endl;
}

bool PostgreSQLConfig::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << logPrefix() << " " << m_config_pos.logtext() << ": number of database connections cannot be 0";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config
