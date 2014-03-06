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
// database audit configuration implementation
//

#include "DBaudit.hpp"
#include "config/valueParser.hpp"
#include "config/configurationTree.hpp"
#include "types/propertyTree.hpp"

namespace _Wolframe {
namespace AAAA {

bool DBauditConfig::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			   const module::ModulesDirectory* /*modules*/ )
{
	using namespace config;

	bool retVal = true;
	bool reqDefined = false;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "required" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_required, Parser::BoolDomain(), &reqDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "database" ))	{
			bool isDefined = ( !m_dbConfig.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_dbConfig, &isDefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
					<< L1it->first << "'";
		}
	}
	return retVal;
}


bool DBauditConfig::check() const
{
	return !m_dbConfig.empty();
}


void DBauditConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << std::endl;
	os << indStr << "   Required: " << (m_required ? "yes" : "no") << std::endl;
	os << indStr << "   Database: " << m_dbConfig << std::endl;
}

}} // namespace _Wolframe::AAAA


