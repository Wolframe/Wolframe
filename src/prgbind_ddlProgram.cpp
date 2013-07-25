/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Implementation of programs for definition of forms in a DDL
///\file prgbind_ddlProgram.cpp

#include "prgbind/ddlProgram.hpp"
#include "prgbind/programLibrary.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prgbind;


bool DDLProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (ext.empty()) return false;
	return boost::iequals( m_constructor->ddlname(), ext.c_str()+1);
}

void DDLProgram::loadProgram( ProgramLibrary& library, db::Database*, const std::string& filename)
{
	try
	{
		const types::NormalizeFunctionMap* typemap = library.formtypemap();
		std::vector<types::FormDescriptionR> forms = m_constructor->compile( utils::readSourceFileContent( filename), typemap);
		std::vector<types::FormDescriptionR>::const_iterator fi = forms.begin(), fe = forms.end();
		for (; fi != fe; ++fi)
		{
			library.defineForm( (*fi)->name(), *fi);
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( e.what()) + " loading program '" + filename + "'");
	}
}


