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
// echo processor implementation
//

#include "logger.hpp"
#include "echoProcessor.hpp"

#include "config/configurationParser.hpp"
#include "config/valueParser.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( EchoProcConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	bool retVal = true;
	bool isDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "idle" ))	{
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.m_timeout ))
				retVal = false;
			isDefined = true;
		}
		else	{
			LOG_WARNING << cfg.logPrefix() << ": unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( ! isDefined )
		cfg.m_timeout = 0;

	return retVal;
}

} // namespace config

bool EchoProcConfig::check() const
{
	return true;
}

void EchoProcConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	os << indStr << "   Idle timeout: " << m_timeout << std::endl;
}

void EchoProcConfig::setCanonicalPathes( const std::string& /*refPath*/ )
{
}


EchoProcContainer::EchoProcContainer( const EchoProcConfig& /*conf*/ )
{
	LOG_NOTICE << "echo processor container created";
}

} // namespace _Wolframe

