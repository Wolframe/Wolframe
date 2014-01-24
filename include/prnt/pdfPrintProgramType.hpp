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
///\brief Program type for printing with simplePDF based on libhpdf (haru)
///\file pdfPrintProgramType.hpp

#ifndef _PRGBIND_SIMPLE_PDF_PROGRAM_TYPE_HPP_INCLUDED
#define _PRGBIND_SIMPLE_PDF_PROGRAM_TYPE_HPP_INCLUDED
#include "prgbind/program.hpp"
#include "prnt/pdfPrinterDocument.hpp"
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace prnt {

///\class SimplePdfPrintProgram
///\brief Program type for printing with simplePDF
class SimplePdfPrintProgram
	:public prgbind::Program
{
public:
	SimplePdfPrintProgram( prnt::CreateDocumentFunc createDocument_)
		:Program(Program::Function)
		,m_createDocument(createDocument_){}
	virtual ~SimplePdfPrintProgram(){}

	virtual bool is_mine( const std::string& filename) const;
	virtual void loadProgram( prgbind::ProgramLibrary& library, db::Database* transactionDB, const std::string& filename);

private:
	prnt::CreateDocumentFunc m_createDocument;
};

}}//namespace
#endif

