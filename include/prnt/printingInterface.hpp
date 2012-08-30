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
///\file prnt/PrintingInterface.hpp
///\brief Defines the interface for printing structures to a string of printable format (e.g. PDF)
#ifndef _Wolframe_PRNT_PRINTER_INTERFACE_HPP_INCLUDED
#define _Wolframe_PRNT_PRINTER_INTERFACE_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace prnt {

///\class PrintingInterface
///\brief Interface for printing forms to a string of printable format (e.g. PDF)
struct PrintingInterface
{
	///\brief Constructor
	///\param[in] name name of the printing interface
	explicit PrintingInterface( const std::string& name_) :m_name(name_) {}
	///\brief Destructor
	virtual ~PrintingInterface(){}

	///\brief Prints a structure to a string
	///\param[in] input input structure
	///\return the printing output as string
	virtual std::string print( const langbind::TypedInputFilterR& input) const=0;

	///\brief Get the name of this printing interface
	const std::string& name() const			{return m_name;}

private:
	std::string m_name;
};

///\brief Reference to a printing interface
typedef types::CountedReference<PrintingInterface> PrintingInterfaceR;

///\param[in] src form description source
typedef PrintingInterfaceR (*CreatePrintingInterfaceFunc)( const std::string& src);

}}//namespace
#endif

