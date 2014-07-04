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
// Oracle configuration parser
//

#include "Oracle.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "serialize/struct/structDescription.hpp"
#include "utils/fileUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

enum {
	DEFAULT_ORACLE_CONNECTIONS = 4,
	DEFAULT_CONNECTION_TIMEOUT = 30,
	DEFAULT_STATEMENT_TIMEOUT = 30000
};

namespace _Wolframe {
namespace db {

OracleConfigStruct::OracleConfigStruct()
	:m_port(0)
	,connectTimeout(DEFAULT_CONNECTION_TIMEOUT)
	,connections(DEFAULT_ORACLE_CONNECTIONS)
	,acquireTimeout(0)
	,statementTimeout(DEFAULT_STATEMENT_TIMEOUT)
{}

const serialize::StructDescriptionBase* OracleConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<OracleConfigStruct>
	{
	ThisDescription()
	{
		(*this)
		( "identifier", &OracleConfigStruct::m_ID)		.mandatory()
		( "host", &OracleConfigStruct::m_host )			.optional()
		( "port", &OracleConfigStruct::m_port )			.optional()
		( "database", &OracleConfigStruct::m_dbName )		.optional()
		( "user", &OracleConfigStruct::m_user )			.optional()
		( "password", &OracleConfigStruct::m_password )		.optional()
		( "sslMode", &OracleConfigStruct::sslMode )		.optional()
		( "sslCert", &OracleConfigStruct::sslCert )		.optional()
		( "sslKey", &OracleConfigStruct::sslKey )		.optional()
		( "sslRootCert", &OracleConfigStruct::sslRootCert )	.optional()
		( "sslCRL", &OracleConfigStruct::sslCRL )		.optional()
		( "connectionTimeout", &OracleConfigStruct::connectTimeout ).optional()
		( "connections", &OracleConfigStruct::connections )	.optional()
		( "acquireTimeout", &OracleConfigStruct::acquireTimeout ).optional()
		( "statementTimeout", &OracleConfigStruct::statementTimeout ).optional()
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}

//***  Oracle configuration functions  **********************************
OracleConfig::OracleConfig( const char* cfgName, const char* logParent, const char* logName )
	: config::NamedConfiguration( cfgName, logParent, logName )
{}

bool OracleConfig::mapValueDomains()
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

bool OracleConfig::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			      const module::ModulesDirectory* /*modules*/ )
{
	try
	{
		config::parseConfigStructure( *static_cast<OracleConfigStruct*>(this), pt);
		m_config_pos = pt.position();
		return mapValueDomains();
	}
	catch (const std::runtime_error& e)
	{
		LOG_FATAL << logPrefix() << e.what();
		return false;
	}
}

void OracleConfig::setCanonicalPathes( const std::string& refPath )
{
	if ( ! sslCert.empty() )	{
		std::string oldPath = sslCert;
		sslCert = utils::getCanonicalPath( sslCert, refPath);
/* Aba: avoid "suggest explicit braces to avoid ambiguous ‘else’ [-Wparentheses]" */
		if ( oldPath != sslCert ) {
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute SSL certificate filename '" << sslCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslKey.empty() )	{
		std::string oldPath = sslKey;
		sslKey = utils::getCanonicalPath( sslKey, refPath );
		if ( oldPath != sslKey ) {
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute SSL key filename '" << sslKey
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslRootCert.empty() )	{
		std::string oldPath = sslRootCert;
		sslRootCert = utils::getCanonicalPath( sslRootCert, refPath );
		if ( oldPath != sslRootCert ) {
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute CA certificate filename '" << sslRootCert
				       << "' instead of '" << oldPath << "'";
		}
	}
	if ( ! sslCRL.empty() )	{
		std::string oldPath = sslCRL;
		sslCRL = utils::getCanonicalPath( sslCRL, refPath );
		if ( oldPath != sslCRL ) {
/*MBa ?!?*/		LOG_NOTICE << logPrefix() << "Using absolute CRL filename '" << sslCRL
				       << "' instead of '" << oldPath << "'";
		}
	}
}

void OracleConfig::print( std::ostream& os, size_t indent ) const
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

bool OracleConfig::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config
