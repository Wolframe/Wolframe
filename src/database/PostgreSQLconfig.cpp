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
#include "config/ConfigurationTree.hpp"

static const unsigned short DEFAULT_POSTGRESQL_CONNECTIONS = 4;
static const unsigned short DEFAULT_CONNECTION_TIMEOUT = 30;

namespace _Wolframe {
namespace db {


bool PostgreSQLconfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			      const module::ModulesDirectory* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined, cTdefined;
	portDefined = connDefined = aTdefined = cTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_ID.empty() );
			std::string id;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				m_ID = id;
		}
		else if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !host.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			bool isDefined = ( !dbName.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, dbName, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !user.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !password.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connectionTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, connectTimeout, &cTdefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, connections,
						Parser::RangeDomain<unsigned short>( 1 ), &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( ! connDefined == 0 )
		connections = DEFAULT_POSTGRESQL_CONNECTIONS;
	if ( ! cTdefined )
		connectTimeout = DEFAULT_CONNECTION_TIMEOUT;

	return retVal;
}

}} // namespace _Wolframe::config
