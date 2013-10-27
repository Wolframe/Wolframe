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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file prnt/printFunction.hpp
///\brief Defines the interface for printing structures to a string of printable format (e.g. PDF)
#ifndef _Wolframe_PRNT_PRINT_FUNCTION_HPP_INCLUDED
#define _Wolframe_PRNT_PRINT_FUNCTION_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/countedReference.hpp"
#include <string>

namespace _Wolframe {
namespace prnt {

///\class PrintFunction
///\brief Interface for printing forms to a string of printable format (e.g. PDF)
struct PrintFunction
{
	typedef langbind::TypedOutputFilter Input;
	typedef types::CountedReference<Input> InputR;

	///\brief Constructor
	explicit PrintFunction(){}
	///\brief Destructor
	virtual ~PrintFunction(){}

	virtual InputR getInput() const=0;
	virtual std::string execute( const Input* i) const=0;

	virtual std::string tostring() const=0;
	virtual const std::string& name() const=0;
};

///\brief Reference to a printing interface
typedef types::CountedReference<PrintFunction> PrintFunctionR;

///\param[in] description print description source
typedef PrintFunction* (*CreatePrintFunction)( const std::string& description);

}}//namespace
#endif

