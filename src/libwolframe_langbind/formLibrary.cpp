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
///\brief Implementation of a library of forms
///\file formLibrary.cpp
#include "langbind/formLibrary.hpp"
#include "utils/miscUtils.hpp"
#include "utils/doctype.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void FormLibrary::addConstructor( const ddl::DDLCompilerR& constructor)
{
	m_constructormap.insert( constructor->ddlname(), constructor);
}

bool FormLibrary::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (ext.empty()) return false;
	types::keymap<ddl::DDLCompilerR>::const_iterator ci = m_constructormap.find( std::string( ext.c_str()+1));
	return (ci != m_constructormap.end());
}

void FormLibrary::loadProgram( const std::string& filename)
{
	if (!m_typemap.get()) throw std::logic_error( "type map not defined in form library");
	try
	{
		std::string ext = utils::getFileExtension( filename);
		if (ext.empty()) std::runtime_error( std::string( "unknown DDL type for file '") + filename + "'");
		types::keymap<ddl::DDLCompilerR>::const_iterator ci = m_constructormap.find( std::string( ext.c_str()+1));
		if (ci == m_constructormap.end()) throw std::runtime_error( std::string( "unknown DDL type for file '") + filename + "'");

		std::vector<ddl::Form> forms = ci->second->compile( utils::readSourceFileContent( filename), m_typemap.get());
		std::vector<ddl::Form>::const_iterator fi = forms.begin(), fe = forms.end();
		for (; fi != fe; ++fi)
		{
			ddl::FormR form( new ddl::Form( *fi));
			std::string name;
			if (fi->doctype())
			{
				name = utils::getIdFromDoctype( fi->doctype());
			}
			else
			{
				name = utils::getFileStem( filename);
			}
			insert( name, form);
		}
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalErrorException( filename, e);
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in form definition file '") + filename + "' :" + err.what());
	}
}


