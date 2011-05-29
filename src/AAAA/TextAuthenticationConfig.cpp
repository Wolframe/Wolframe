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
//
//

#include "logger.hpp"
#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"
#include "authenticator.hpp"
#include "TextFileAuthentication.hpp"

#include "boost/algorithm/string.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( AAAA::TextFileAuthConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& node )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	if ( boost::algorithm::iequals( node, "file" ) || boost::algorithm::iequals( node, "filename" ))	{
		bool isDefined = ( !cfg.m_file.empty() );
		if ( !Parser::getValue( cfg.logPrefix().c_str(), node.c_str(),
					pt.get_value<std::string>(), cfg.m_file, &isDefined ))
			retVal = false;
		else	{
			if ( ! boost::filesystem::path( cfg.m_file ).is_absolute() )
				LOG_WARNING << cfg.logPrefix() << "authentication file path is not absolute: "
					    << cfg.m_file;
		}
	}
	else	{
		LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '" << node << "'";
	}
	return retVal;
}

}} // namespace _Wolframe::config
