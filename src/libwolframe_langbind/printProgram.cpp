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
///\brief Implementation of programs for printing
///\file printProgram.cpp
#include "langbind/printProgram.hpp"
#include "utils/miscUtils.hpp"
#include "config/programBase.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void PrintProgram::addConstructor( const module::PrintFunctionConstructorR& constructor)
{
	std::string domain = constructor->programFileType();
	m_constructormap.insert( domain, constructor);
}

bool PrintProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (ext.empty()) return false;
	types::keymap<module::PrintFunctionConstructorR>::const_iterator ci = m_constructormap.find( std::string( ext.c_str()+1));
	return (ci != m_constructormap.end());
}

void PrintProgram::loadProgram( const std::string& filename)
{
	try
	{
		std::string ext = utils::getFileExtension( filename);
		if (ext.empty()) std::runtime_error( std::string( "unknown print layout type for file '") + filename + "'");
		types::keymap<module::PrintFunctionConstructorR>::const_iterator ci = m_constructormap.find( std::string( ext.c_str()+1));
		if (ci == m_constructormap.end()) throw std::runtime_error( std::string( "unknown print layout type for file '") + filename + "'");
		prnt::PrintFunctionR function( ci->second->object( utils::readSourceFileContent( filename)));
		std::string name = function->name();
		if (name.empty())
		{
			name = utils::getFileStem( filename);
		}
		insert( name, function);
	}
	catch (const config::PositionalErrorException& e)
	{
		throw config::PositionalErrorException( filename, e);
	}
	catch (const std::runtime_error& err)
	{
		throw std::runtime_error( std::string( "error in print layout definition file '") + filename + "' :" + err.what());
	}
}


