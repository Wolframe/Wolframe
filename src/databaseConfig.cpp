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
// database configuration functions
//

#include "database.hpp"
#include "connectionBase.hpp"
#include "config/valueParser.hpp"		// *** !
#include "configHelpers.hpp"			// *** !
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

static const unsigned short DEFAULT_DB_CONNECTIONS = 4;

namespace _Wolframe	{
namespace db	{

Configuration::Configuration() : _Wolframe::config::ConfigurationBase( "Database Server" )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}


void Configuration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	if ( host.empty())
		os << "   Database host: local unix domain socket" << std::endl;
	else
		os << "   Database host: " << host << ":" << port << std::endl;
	os << "   Database name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
	os << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
		 << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	os << "   Database connections: " << connections << std::endl;
	os << "   Acquire database connection timeout: " << acquireTimeout << std::endl;
}


/// Check if the database configuration makes sense
bool Configuration::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << "Invalid number of connections: " << connections;
		return false;
	}
	return true;
}


bool Configuration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, host ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, port, Parser::RangeDomain<unsigned short>( 1 )))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, name ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, user ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, password ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, connections ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( displayName().c_str(), *L1it, acquireTimeout ))
				retVal = false;
		}
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <"
				    << L1it->first << ">";
		}
	}
	if ( connections == 0 )
		connections = DEFAULT_DB_CONNECTIONS;

	return retVal;
}

}} // namespace _Wolframe::db
