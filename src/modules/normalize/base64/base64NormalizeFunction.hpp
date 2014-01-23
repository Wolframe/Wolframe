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
///\file base64NormalizeFunction.hpp
///\brief Normalize function for endode/decode for base64
#ifndef _LANGBIND_BASE64_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _LANGBIND_BASE64_NORMALIZE_FUNCTION_HPP_INCLUDED
#include "types/normalizeFunction.hpp"
#include <string>

namespace _Wolframe {
namespace langbind {

class Base64DecodeFunction :public types::NormalizeFunction
{
public:
	Base64DecodeFunction(){}
	virtual types::Variant execute( const types::Variant& inp) const;
	virtual const char* name() const {return "decode";}
};

class Base64EncodeFunction :public types::NormalizeFunction
{
public:
	Base64EncodeFunction(){}
	virtual types::Variant execute( const types::Variant& inp) const;
	virtual const char* name() const {return "encode";}
};

}}
#endif
