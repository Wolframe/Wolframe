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
///\brief Program type for testing simplePDF
///\file pdfPrintProgramType.cpp
#include "pdfPrintProgramType.hpp"
#include "pdfPrinter.hpp"
#include "utils/fileUtils.hpp"
#include "langbind/formFunction.hpp"
#include "prgbind/programLibrary.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::prnt;

bool SimplePdfPrintProgram::is_mine( const std::string& filename) const
{
	std::string ext = utils::getFileExtension( filename);
	if (boost::algorithm::iequals( ext, ".sprn")) return true;
	return false;
}

void SimplePdfPrintProgram::loadProgram( prgbind::ProgramLibrary& library, db::Database*, const std::string& filename)
{
	std::string src( utils::readSourceFileContent( filename));
	HaruPdfPrintFunction* prntfunc = new HaruPdfPrintFunction( src, m_createDocument);
	langbind::FormFunctionR func( prntfunc);
	
	library.defineFormFunction( prntfunc->name(), func);
}


