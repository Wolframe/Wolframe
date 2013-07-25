/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
//
//

#include "logger-v1.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "SaslAuth.hpp"

#include "boost/algorithm/string.hpp"
#include <boost/filesystem.hpp>

using namespace _Wolframe::utils;

namespace _Wolframe {
namespace AAAA {

bool SaslAuthConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
				const module::ModulesDirectory* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_identifier.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_identifier, &isDefined )) {
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "service" ))	{
			bool isDefined = ( !m_service.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_service, &isDefined )) {
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "confPath" ))	{
			bool isDefined = ( !m_confPath.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_confPath, &isDefined )) {
				retVal = false;
			}
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
					<< L1it->first << "'";
		}
	}
	return retVal;
}

}} // namespace _Wolframe::config
