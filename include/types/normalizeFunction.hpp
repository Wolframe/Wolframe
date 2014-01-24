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
///\file types/normalizeFunction.hpp
///\brief Normalize function definition

#ifndef _Wolframe_TYPES_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _Wolframe_TYPES_NORMALIZE_FUNCTION_HPP_INCLUDED
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

//\class Variant
//\brief Forward declaration
class Variant;

//\class NormalizeFunction
//\brief Basic normalization function for atomic values (variant type)
struct NormalizeFunction
{
	virtual ~NormalizeFunction(){}
	virtual const char* name() const=0;
	virtual Variant execute( const Variant& i) const=0;
};

//\brief Shared ownership reference to normalizatio functions for atomic values (variant type)
typedef boost::shared_ptr<NormalizeFunction> NormalizeFunctionR;

//\class NormalizeFunctionMap
//\brief Map of basic normalization functions for atomic values (variant type)
struct NormalizeFunctionMap
{
	virtual ~NormalizeFunctionMap(){}
	virtual const NormalizeFunction* get( const std::string& name) const=0;
};

//\brief Shared ownership reference to map of basic normalization functions for atomic values (variant type)
typedef boost::shared_ptr<NormalizeFunctionMap> NormalizeFunctionMapR;


class NormalizeResourceHandle
{
public:
	NormalizeResourceHandle(){}
	virtual ~NormalizeResourceHandle(){}
};

typedef NormalizeResourceHandle* (*CreateNormalizeResourceHandleFunction)();

///\param[in,out] rshnd normalization resources handle
///\param[in] description transaction description source
typedef types::NormalizeFunction* (*CreateNormalizeFunction)( NormalizeResourceHandle* reshnd, const std::string& arg);

}}//namespace
#endif

