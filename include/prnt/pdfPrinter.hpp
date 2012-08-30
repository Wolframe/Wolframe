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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt/pdfPrinter.hpp
///\brief Defines the prnt::PrintingInterface for an implementation based on libhpdf and some form definition
#ifndef _Wolframe_PRNT_PDF_PRINTER_HPP_INCLUDED
#define _Wolframe_PRNT_PDF_PRINTER_HPP_INCLUDED
#include "prnt/printingInterface.hpp"
#include "types/countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace prnt {

///\class PrintingInterface
///\brief Interface for printing forms to a string of printable format (e.g. PDF)
class PdfPrinter :public PrintingInterface
{
public:
	///\brief Constructor
	///\param[in] name name of the printing interface
	PdfPrinter( const std::string& name_, const std::string& description);
	///\brief Destructor
	virtual ~PdfPrinter(){}

	///\brief Implements PrintingInterface::print(const langbind::TypedInputFilterR& input)const;
	virtual std::string print( const langbind::TypedInputFilterR& input) const;

private:
	struct PdfPrinterImpl;
	PdfPrinterImpl* m_impl;
};

///\brief Reference to a printing interface
typedef types::CountedReference<PrintingInterface> PrintingInterfaceR;

///\param[in] src form description source
PrintingInterfaceR createPdfPrinter( const std::string& description);

}}//namespace
#endif

