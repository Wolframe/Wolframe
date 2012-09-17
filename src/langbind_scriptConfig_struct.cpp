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
///\file langbind_scriptConfig_struct.cpp
///\brief Implementation of the data structures of a script environment configuration
#include "langbind/scriptConfig_struct.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const serialize::StructDescriptionBase* ScriptCommandConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<ScriptCommandConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&ScriptCommandConfigStruct::name)
			( "file",		&ScriptCommandConfigStruct::file)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* ScriptEnvironmentConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<ScriptEnvironmentConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "script",		&ScriptEnvironmentConfigStruct::script)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

void ScriptEnvironmentConfigStruct::setCanonicalPathes( const std::string& referencePath)
{
	{
		std::vector<ScriptCommandConfigStruct>::iterator itr=script.begin(),end=script.end();
		for (;itr!=end; ++itr)
		{
			itr->file = utils::getCanonicalPath( itr->file, referencePath);
		}
	}
}

boost::property_tree::ptree ScriptEnvironmentConfigStruct::toPropertyTree() const
{
	return config::structureToPropertyTree( *this);
}

void ScriptEnvironmentConfigStruct::initFromPropertyTree( const boost::property_tree::ptree& pt)
{
	config::parseConfigStructure( *this, pt);
}

bool ScriptEnvironmentConfigStruct::check() const
{
	for (std::vector<ScriptCommandConfigStruct>::const_iterator ii=script.begin(), ee=script.end(); ii != ee; ++ii)
	{
		if (!utils::fileExists( ii->file)) return false;
	}
	return true;
}

void ScriptEnvironmentConfigStruct::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent+1, '\t');
	indentstr[0] = '\n';
	std::string rt( config::structureToString( *this));
	boost::replace_all( rt, "\n", indentstr);
	os << rt;
}


