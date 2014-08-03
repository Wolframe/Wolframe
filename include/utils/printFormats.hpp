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
/// \brief Templates describing printing of structures
/// \file utils/printFormats.hpp
#ifndef _Wolframe_PRINT_FORMATS_HPP_INCLUDED
#define _Wolframe_PRINT_FORMATS_HPP_INCLUDED
#include <string>
#include "types/variant.hpp"

namespace _Wolframe {
namespace utils {

/// \class PrintFormat
/// \brief Structure with info for uniform printing for a type of message (log format as compact one liner, text as multi liner)
struct PrintFormat
{
	const char* indent;		///< string to print for indentiation per level of hierarchy for a new item (after 'newitem')
	const char* newitem;		///< string to print for a new item
	const char* openstruct;		///< string to print as start marker of a structure
	const char* closestruct;	///< string to print as end marker of a structure
	const char* endheader;		///< string to print as end marker of a header
	const char* startvalue;		///< string to print as start marker of a value
	const char* endvalue;		///< string to print as end marker of a value
	const char* decldelimiter;	///< string to print as delimiter of declarations
	const char* itemdelimiter;	///< string to print as delimiter of values
	const char* assign;		///< assignment operator for attributes
	std::size_t maxitemsize;	///< maximum size of printed items
};

/// \brief Get the print format for logger output
const PrintFormat* logPrintFormat();
/// \brief Get the print format for readable multi line output similar to property tree format
const PrintFormat* ptreePrintFormat();

/// \brief Get a string to log out of a variant structure with a restriction of the size of the atomic elements
std::string getLogString( const types::Variant& val, std::size_t maxsize=60);

}}
#endif
