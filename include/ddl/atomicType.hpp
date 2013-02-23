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
///\file ddl/atomicType.hpp
///\brief Defines a non intrusive atomic type for the DDLs used for forms

#ifndef _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#define _Wolframe_DDL_ATOMICTYPE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include <string>
#include <stdexcept>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <limits>
#include <boost/utility/enable_if.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace ddl {

struct NormalizeFunction
{
	virtual ~NormalizeFunction(){}
	virtual const char* name() const=0;
	virtual std::string execute( const std::string& i) const=0;
};

struct TypeMap
{
	virtual ~TypeMap(){}
	virtual const NormalizeFunction* getType( const std::string& name) const=0;
};

typedef types::CountedReference<TypeMap> TypeMapR;


///\class AtomicType
///\brief atomic type for DDL language binding
class AtomicType
{
public:
	///\brief Constructor
	AtomicType( const NormalizeFunction* t=0)
		:m_type(t){}

	///\brief Copy constructor
	///\param[in] o element to copy
	AtomicType( const AtomicType& o)
		:m_type(o.m_type),m_value(o.m_value){}

	///\brief Reset the value
	void clear()
	{
		m_type = 0;
		m_value.clear();
	}

	///\brief Setter method
	///\param[in] val element to assign
	///\return true on success, false if the type check fails
	void set( const std::string& val)
	{
		m_value = (m_type)?m_type->execute( val):val;
	}

	///\brief Get the value of this as string
	///\return the value of this as string
	const std::string& value() const
	{
		return m_value;
	}

	///\brief Get the type of this as string
	///\return the type of this as string
	const char* type() const
	{
		return m_type?m_type->name():"string";
	}

private:
	const NormalizeFunction* m_type;	//< type of this
	std::string m_value;			//< value of this
	bool assign_string( const std::string& val);
};

}}//namespace
#endif

