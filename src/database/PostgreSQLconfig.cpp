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
// PostgreSQL configuration parser
//

#include "PostgreSQL.hpp"
#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"

static const unsigned short DEFAULT_POSTGRESQL_CONNECTIONS = 4;
static const unsigned short DEFAULT_CONNECTION_TIMEOUT = 30;

namespace _Wolframe {
namespace config {

/// Specialization of the ConfigurationParser::parse for the PostgreSQL configuration
template<>
bool ConfigurationParser::parse( db::PostgreSQLconfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/,
				 const module::ModulesConfiguration* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined, cTdefined;
	portDefined = connDefined = aTdefined = cTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !cfg.m_ID.empty() );
			std::string id;
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				cfg.m_ID = id;
		}
		else if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !cfg.host.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			bool isDefined = ( !cfg.dbName.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.dbName, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !cfg.user.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !cfg.password.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connectionTimeout" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.connectTimeout, &cTdefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.connections,
						Parser::RangeDomain<unsigned short>( 1 ), &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( ! connDefined == 0 )
		cfg.connections = DEFAULT_POSTGRESQL_CONNECTIONS;
	if ( ! cTdefined )
		cfg.connectTimeout = DEFAULT_CONNECTION_TIMEOUT;

	return retVal;
}

}} // namespace _Wolframe::config
