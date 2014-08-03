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
///\brief Configuration tree for the configuration parser
///\file config/configurationTree.hpp

#ifndef _CONFIGURATION_TREE_HPP_INCLUDED
#define _CONFIGURATION_TREE_HPP_INCLUDED
#include "types/propertyTree.hpp"
#include <string>

namespace _Wolframe {
namespace config {

/// \class ConfigurationTree
/// \brief Configuration structure as tree
class ConfigurationTree
	:public types::PropertyTree
{
public:
	/// \brief Default constructor
	ConfigurationTree(){}
	/// \brief Constructor
	ConfigurationTree( const boost::property_tree::ptree& pt, const std::string& filename)
		:types::PropertyTree( pt, filename){}
	/// \brief Constructor
	ConfigurationTree( const types::PropertyTree& pt)
		:types::PropertyTree( pt){}
};

typedef types::PropertyTree::Node ConfigurationNode;

}} // namespace _Wolframe::config
#endif // _CONFIGURATION_TREE_HPP_INCLUDED
