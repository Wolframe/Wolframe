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

#include "OracleConfig.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "serialize/struct/structDescription.hpp"
#include "utils/fileUtils.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

const serialize::StructDescriptionBase* OracleConfig::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<OracleConfig>
	{
	ThisDescription()
	{
		(*this)
		( "identifier", &OracleConfig::m_ID)		.mandatory()
		( "host", &OracleConfig::m_host )		.optional()
		( "port", &OracleConfig::m_port )		.optional()
		( "database", &OracleConfig::m_dbName )		.optional()
		( "user", &OracleConfig::m_user )		.optional()
		( "password", &OracleConfig::m_password )	.optional()
		( "connections", &OracleConfig::m_connections )	.optional()
		( "acquireTimeout", &OracleConfig::m_acquireTimeout ).optional()
		;
	}
	};
	static const ThisDescription rt;
	return &rt;
}

bool OracleConfig::mapValueDomains()
{
	bool retVal = true;
	if (m_port == 0)
	{
		LOG_FATAL << logPrefix() << " " << m_config_pos.logtext() << ": port must be defined as a non zero non negative number";
		retVal = false;
	}
	return retVal;
}

bool OracleConfig::parse( const config::ConfigurationNode& pt, const std::string& node,
			      const module::ModulesDirectory* modules )
{
	bool rt;
	rt = _Wolframe::serialize::DescriptiveConfiguration::parse( pt, node, modules);
	if( rt ) {
		rt = mapValueDomains( );
	}
	return rt;
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

	if ( m_acquireTimeout == 0 )
		os << indStr << "   Acquire database connection timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Acquire database connection timeout: " << m_acquireTimeout << "s" << std::endl;
}

bool OracleConfig::check() const
{
	if ( m_connections == 0 )	{
		LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}

}} // namespace _Wolframe::config
