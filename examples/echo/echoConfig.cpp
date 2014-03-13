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
// echo configuration functions
//

#include "handlerConfig.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <ostream>

static const unsigned short DEFAULT_TIMEOUT = 180;

namespace _Wolframe {

bool EchoConfiguration::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			       const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;
	bool isDefined = false;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "idle" ))	{
			if ( !config::Parser::getValue( logPrefix().c_str(), *L1it, timeout ))
				retVal = false;
			isDefined = true;
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( ! isDefined )
		timeout = DEFAULT_TIMEOUT;

	return retVal;
}

void EchoConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Idle timeout: " << timeout << std::endl;
}


/// Check if the database configuration makes sense
bool EchoConfiguration::check() const
{
	return true;
}

} // namespace _Wolframe
