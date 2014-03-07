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
//\file types/propertyTree.cpp
//\brief Implementation of a key value tree based on boost::property_tree::ptree with position info for better error reporting
#include "types/propertyTree.hpp"
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::types;

PropertyTree::FileName types::PropertyTree::getFileName( const std::string& name)
{
	char* cc = (char*)std::malloc( name.size()+1);
	if (!cc) throw std::bad_alloc();
	std::memcpy( cc, name.c_str(), name.size()+1);
	return boost::shared_ptr<char>( cc, std::free);
}

std::string PropertyTree::Position::logtext() const
{
	if (!filename()) return std::string();
	return std::string("in file '") + filename() + "' at line " + boost::lexical_cast<std::string>(m_line) + " column " + boost::lexical_cast<std::string>(m_column);
}

PropertyTree::Node::Node( const boost::property_tree::ptree& pt)
{
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		Parent::add_child( pi->first, Node( pi->second));
	}
	if (!pt.data().empty())
	{
		Parent::put_value( Value( pt.data()));
	}
}

void PropertyTree::Node::recursiveSetFileName( Parent& pt, const FileName& filename)
{
	Parent::iterator pi = pt.begin(), pe = pt.end();
	for (; pi != pe; ++pi)
	{
		recursiveSetFileName( pi->second, filename);
	}
	pt.data().position.setFileName( filename);
}

