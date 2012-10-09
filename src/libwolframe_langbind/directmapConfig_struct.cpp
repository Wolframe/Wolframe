/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\file langbind_directmapConfig_struct.cpp
///\brief Implementation of the data structures of a directmap command handler configuration
#include "langbind/directmapConfig_struct.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const serialize::StructDescriptionBase* DirectmapCommandConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<DirectmapCommandConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&DirectmapCommandConfigStruct::name)
			( "function",		&DirectmapCommandConfigStruct::function)
			( "filter",		&DirectmapCommandConfigStruct::filter)
			( "inputform",		&DirectmapCommandConfigStruct::inputform)
			( "outputform",		&DirectmapCommandConfigStruct::outputform)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* DirectmapConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<DirectmapConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "command",		&DirectmapConfigStruct::command)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

void DirectmapConfigStruct::setCanonicalPathes( const std::string&)
{}

boost::property_tree::ptree DirectmapConfigStruct::toPropertyTree() const
{
	return config::structureToPropertyTree( *this);
}

void DirectmapConfigStruct::initFromPropertyTree( const boost::property_tree::ptree& pt)
{
	config::parseConfigStructure( *this, pt);
}

bool DirectmapConfigStruct::check() const
{
	return true;
}

void DirectmapConfigStruct::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent+1, '\t');
	indentstr[0] = '\n';
	std::string rt( config::structureToString( *this));
	boost::replace_all( rt, "\n", indentstr);
	os << rt;
}


