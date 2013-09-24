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
///\file utils/conversions.hpp
///\brief Conversions of atomic types

#ifndef _Wolframe_UTILS_CONVERSIONS_HPP_INCLUDED
#define _Wolframe_UTILS_CONVERSIONS_HPP_INCLUDED
#include <string>
#include "types/variant.hpp"

namespace _Wolframe {
namespace utils {

std::string tostring_cast( types::Variant::Data::UInt val);
std::string tostring_cast( types::Variant::Data::Int val);
types::Variant::Data::UInt touint_cast( const std::string& val);
types::Variant::Data::Int toint_cast( const std::string& val);

}}//namespace
#endif

