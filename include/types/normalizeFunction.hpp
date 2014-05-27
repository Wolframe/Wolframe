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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
/// \file types/normalizeFunction.hpp
/// \brief Normalize function definition

#ifndef _Wolframe_TYPES_NORMALIZE_FUNCTION_HPP_INCLUDED
#define _Wolframe_TYPES_NORMALIZE_FUNCTION_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

/// \class Variant
/// \brief Forward declaration
class Variant;

/// \class NormalizeFunction
/// \brief Basic normalization function for atomic values (variant type)
class NormalizeFunction
{
public:
	virtual ~NormalizeFunction(){}
	virtual const char* name() const=0;
	virtual Variant execute( const Variant& i) const=0;
	virtual NormalizeFunction* copy() const=0;
};

/// \brief Shared ownership reference to normalization function for atomic values (variant type)
typedef boost::shared_ptr<NormalizeFunction> NormalizeFunctionR;


class NormalizeResourceHandle
{
public:
	NormalizeResourceHandle(){}
	virtual ~NormalizeResourceHandle(){}
};

typedef boost::shared_ptr<NormalizeResourceHandle> NormalizeResourceHandleR;


typedef const NormalizeResourceHandleR& (*GetNormalizeResourceHandle)();
typedef NormalizeFunction* (*CreateNormalizeFunction)( NormalizeResourceHandle* reshnd, const std::vector<types::Variant>& arg);

/// \class NormalizeFunctionType
/// \brief Class of basic normalization functions instantiated by arguments
class NormalizeFunctionType
{
public:
	explicit NormalizeFunctionType( CreateNormalizeFunction c=0)
		:m_createFunction(c){}
	explicit NormalizeFunctionType( CreateNormalizeFunction c, const NormalizeResourceHandleR& resources_)
		:m_createFunction(c),m_resources(resources_){}
	NormalizeFunctionType( const NormalizeFunctionType& o)
		:m_createFunction(o.m_createFunction),m_resources(o.m_resources){}
	~NormalizeFunctionType(){}

	NormalizeFunction* createFunction( const std::vector<types::Variant>& arg) const
	{
		if (m_createFunction)
		{
			return m_createFunction( m_resources.get(), arg);
		}
		else
		{
			return 0;
		}
		
	}
private:
	CreateNormalizeFunction m_createFunction;
	NormalizeResourceHandleR m_resources;
};




/// \class NormalizeFunctionMap
/// \brief Map of basic normalization functions for atomic values (variant type)
struct NormalizeFunctionMap
{
	virtual ~NormalizeFunctionMap(){}
	virtual const NormalizeFunction* get( const std::string& name) const=0;
};

/// \brief Shared ownership reference to map of basic normalization functions for atomic values (variant type)
typedef boost::shared_ptr<NormalizeFunctionMap> NormalizeFunctionMapR;

}}//namespace
#endif

