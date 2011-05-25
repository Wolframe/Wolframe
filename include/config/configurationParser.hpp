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
// configuration parser base class
//
///\note	This file is separated from configurationBase.hpp
///		in order to keep boost/property_tree/ptree.hpp away
///		from the dependencies of the configuration structures

#ifndef _CONFIG_PARSER_HPP_INCLUDED
#define _CONFIG_PARSER_HPP_INCLUDED

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "config/configurationBase.hpp"

namespace _Wolframe {
namespace config {

class ConfigurationParser
{
public:
	/// Parse the configuration section
	///\param[in]	pt		property tree node
	///\param[in]	nodeName	the label of the node. It should be
	///				the same (case insensitive) as it->first
	template<typename T>
	static bool parse( T& configuration,
			   const boost::property_tree::ptree& pt, const std::string& nodeName );
	template<typename T>
	static bool parseBase( ConfigurationBase& configuration,
			       const boost::property_tree::ptree& pt, const std::string& nodeName )
	{
		return parse( dynamic_cast<T&>( configuration ), pt, nodeName );
	}
};

}} // namespace _Wolframe::config

#endif // _CONFIG_PARSER_HPP_INCLUDED
