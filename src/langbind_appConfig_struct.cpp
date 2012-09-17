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
///\file langbind_appConfig_struct.cpp
///\brief Implementation of the data structures of the processor environment configuration
#include "langbind/appConfig_struct.hpp"
#include "config/structSerialize.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

const serialize::StructDescriptionBase* DDLFormConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<DDLFormConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "DDL",		&DDLFormConfigStruct::DDL)
			( "file",		&DDLFormConfigStruct::file)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* PrintLayoutConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<PrintLayoutConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&PrintLayoutConfigStruct::name)
			( "type",		&PrintLayoutConfigStruct::type)
			( "file",		&PrintLayoutConfigStruct::file)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* TransactionFunctionConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<TransactionFunctionConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&TransactionFunctionConfigStruct::name)
			( "type",		&TransactionFunctionConfigStruct::type)
			( "call",		&TransactionFunctionConfigStruct::call)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* EnvironmentConfigStruct::getStructDescription()
{
	struct ThisDescription :public serialize::StructDescription<EnvironmentConfigStruct>
	{
		ThisDescription()
		{
			(*this)
			( "form",		&EnvironmentConfigStruct::form)
			( "printlayout",	&EnvironmentConfigStruct::printlayout)
			( "transaction",	&EnvironmentConfigStruct::transaction)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

void EnvironmentConfigStruct::setCanonicalPathes( const std::string& referencePath)
{
	{
		std::vector<DDLFormConfigStruct>::iterator itr=form.begin(),end=form.end();
		for (;itr!=end; ++itr)
		{
			itr->file = utils::getCanonicalPath( itr->file, referencePath);
		}
	}
	{
		std::vector<PrintLayoutConfigStruct>::iterator itr=printlayout.begin(),end=printlayout.end();
		for (;itr!=end; ++itr)
		{
			itr->file = utils::getCanonicalPath( itr->file, referencePath);
		}
	}
}

boost::property_tree::ptree EnvironmentConfigStruct::toPropertyTree() const
{
	return config::structureToPropertyTree( *this);
}

void EnvironmentConfigStruct::initFromPropertyTree( const boost::property_tree::ptree& pt)
{
	config::parseConfigStructure( *this, pt);
}

bool EnvironmentConfigStruct::check() const
{
	for (std::vector<DDLFormConfigStruct>::const_iterator ii=form.begin(), ee=form.end(); ii != ee; ++ii)
	{
		if (!utils::fileExists( ii->file)) return false;
	}
	for (std::vector<PrintLayoutConfigStruct>::const_iterator ii=printlayout.begin(), ee=printlayout.end(); ii != ee; ++ii)
	{
		if (!utils::fileExists( ii->file)) return false;
	}
	return true;
}

void EnvironmentConfigStruct::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent+1, '\t');
	indentstr[0] = '\n';
	std::string rt( config::structureToString( *this));
	boost::replace_all( rt, "\n", indentstr);
	os << rt;
}


