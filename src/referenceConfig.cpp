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
// reference configuration
//

#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"
#include "config/reference.hpp"

#include "logger.hpp"
#include <ostream>

namespace _Wolframe {
namespace config {

/// Specialization of the ConfigurationParser::parse for the reference configuration
template<>
bool ConfigurationParser::parse( ReferenceConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& node,
				 const module::ModulesDirectory* /*modules*/ )
{
	bool labelDefined = ( ! cfg.m_ref.empty() );
	if ( !config::Parser::getValue( cfg.logPrefix().c_str(), node.c_str(),
					pt.get_value<std::string>(), cfg.m_ref, &labelDefined ))
		return false;
	if ( cfg.m_ref.empty() )	{
		LOG_ERROR << cfg.logPrefix() << "Reference is empty";
		return false;
	}
	return true;
}


bool ReferenceConfig::check() const
{
	if ( m_ref.empty() )	{
		LOG_ERROR << "Reference is empty";
		return false;
	}
	return true;
}


void ReferenceConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": " << (m_ref.empty() ? "Undefined!" : m_ref) << std::endl;
}

}} // namespace _Wolframe::config

