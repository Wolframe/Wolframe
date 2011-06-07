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

#ifndef _Wolframe_CONFIG_STRUCTURE_PRINTER_HPP_INCLUDED
#define _Wolframe_CONFIG_STRUCTURE_PRINTER_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "config/traits.hpp"
#include "logger/logLevel.hpp"

namespace _Wolframe {
namespace config {

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

template <typename T>
static typename boost::enable_if_c<boost::is_same<Category<T>,struct_>::value,const void*>::type
findElement_( const char* type, const char* name, const T& value)
{
	const DescriptionBase* dd = value.description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;
	const char* vv = (const char*)(const void*)(&value);

	for (itr=dd->m_ar.begin(),end=dd->m_ar.end(); itr != end; ++itr)
	{
		if (strcmp( itr->m_type.c_str(), type) == 0)
		{
			if (strcmp( itr->m_name.c_str(), name) == 0)
			{
				return (const void*)(vv + itr->m_ofs);
			}
			else
			{

			}
			const std::string* pp = (const std::string*)(const void*)(vv + itr->m_ofs);
			return (strcmp( name, pp->c_str()) == 0);
		}
	}
	return false;
}

template <typename T>
static typename boost::enable_if_c<boost::is_same<Category<T>,vector_>::value,const void*>::type
findElement_( const char* type, const char* name, const T& value)
{
	return false;
}

template <typename T>
static typename boost::disable_if_c< boost::is_same<Category<T>,vector_>::value || boost::is_same<Category<T>,struct_>::value,const void*>::type
findElement_( const char*, const char*, const T&)
{
	return false;
}

template <typename Element>
struct ElementPointer
{
	static bool matchesElement( const char* name, const void* value)
	{
		return matchesElement_( name, *(const Element*)value);
	}
	static const Element* findElement( const char* name, const void* value)
	{
		const Element* ee = *(const Element*)value;
		const char* type = typeid(Element).name();
		return (Element*)findElement_( type, name, ee);
	}
};

}}// end namespace
#endif

