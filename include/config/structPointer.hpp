/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
/// \file config/structPrinter.hpp
/// \brief Prints the configuration structure

#ifndef _Wolframe_CONFIG_STRUCTURE_POINTER_HPP_INCLUDED
#define _Wolframe_CONFIG_STRUCTURE_POINTER_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "config/traits.hpp"
#include "logger/logLevel.hpp"

namespace _Wolframe {
namespace config {

#if 0
/// \brief returns true, if the structure T matches to name
template <typename T>
static typename boost::enable_if_c<boost::is_same<Category<T>,struct_>::value,bool>::type
matchesElement_( const char* name, const T& value)
{
	const DescriptionBase* dd = value.description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;
	const char* vv = (const char*)(const void*)(&value);

	for (itr=dd->m_ar.begin(),end=dd->m_ar.end(); itr != end; ++itr)
	{
		if (strcmp( itr->m_type.c_str(), "std::string") == 0)
		{
			const std::string* pp = (const std::string*)(const void*)(vv + itr->m_ofs);
			return (strcmp( name, pp->c_str()) == 0);
		}
	}
	return false;
}

template <typename T>
static typename boost::disable_if_c<boost::is_same<Category<T>,struct_>::value,bool>::type
matchesElement_( const char* , const T&)
{
	return false;
}

/// \brief findElement_ for struct_
template <typename T>
static typename boost::enable_if_c<boost::is_same<Category<T>,struct_>::value,const void*>::type
findElement_( const char* type, const char* name, const T& value)
{
	const DescriptionBase* dd = value.description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;
	const char* vv = (const char*)(const void*)(&value);
	const void* rt = 0;

	for (itr=dd->m_ar.begin(),end=dd->m_ar.end(); itr != end; ++itr)
	{
		const void* ee = (const void*)(vv + itr->m_ofs);
		if (strcmp( itr->m_type.c_str(), type) == 0)
		{
			if (strcmp( itr->m_name.c_str(), name) == 0 || itr->m_matches( name, ee))
			{
				if (rt) throw std::logical_error( "ambiguus reference");
				rt = ee;
			}
		}
		else
		{
			const void* rr = itr->m_find( type, name, ee);
			if (rr)
			{
				if (rt) throw std::logical_error( "ambiguus reference");
				rt = rr;
			}
		}
	}
	return rt;
}

/// \brief findElement_ for vector_ of struct_
template <typename T>
static typename boost::enable_if_c<boost::is_same<Category<T>,vector_>::value && boost::is_same<Category<typename T::value_type>,struct_>::value,const void*>::type
findElement_( const char* type, const char* name, const T& value)
{
	const void* rt = 0;
	const char* elemtype = typeid( typename T::value_type).name();
	if (strcmp( type, elemtype) == 0)
	{
		T::const_iterator itr,end;
		for (itr=value.begin(),end=value.end(); itr!=end; ++itr)
		{
			if (matchesElement_( name, &*itr))
			{
				if (rt) throw std::logical_error( "ambiguus reference");
				rt = (const void*)&*itr;
			}
		}
	}
	else
	{
		T::const_iterator itr,end;
		for (itr=value.begin(),end=value.end(); itr!=end; ++itr)
		{
			const void* rr = findElement_( type, name, &*itr);
			if (rr)
			{
				if (rt) throw std::logical_error( "ambiguus reference");
				rt = rr;
			}
		}
	}
	return rt;
}

/// \brief findElement_ for non struct_ and non vector_ of struct_
template <typename T>
static typename boost::disable_if_c<
			boost::is_same<Category<T>,struct_>::value
			|| (boost::is_same<Category<T>,vector_>::value && boost::is_same<Category<typename T::value_type>,struct_>::value)
		,const void*>::type
findElement_( const char*, const char*, const T&)
{
	return false;
}
#endif

template <typename Element>
struct ElementPointer
{
//[-]	static bool matchesElement( const char* name, const void* value)
	static bool matchesElement( const char* , const void*)
	{
		return true;
//[-]		return matchesElement_( name, *(const Element*)value);
	}
//[-]	static const void* findElement( const char* name, const void* value)
	static const void* findElement( const char* , const void* )
	{
//[-]		const Element* ee = *(const Element*)value;
//[-]		const char* type = typeid(Element).name();
//[-]		return findElement_( type, name, ee);
		return 0;
	}
};

}}// end namespace
#endif

