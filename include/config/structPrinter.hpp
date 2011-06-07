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

template <typename Element>
struct ElementPrinter
{
	static void print( std::ostream& out, const char* name, const void* st, std::size_t ofs, unsigned int indent)
	{
		printElement( out, name, *reinterpret_cast<const Element*>((char*)st + ofs), indent);
	}
};

static void print_indent( std::ostream& out, unsigned int nn)
{
	static const char INDENT[3] = "\t";
	unsigned int ii;
	for (ii=0; ii<nn; ii++) out << INDENT;
}

/// \brief prints a structure according its description
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::struct_& , unsigned int indent)
{
	const DescriptionBase* d = value.description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;
	print_indent( out, indent);
	out << std::string(name?name:"") << " {" << std::endl;

	for (itr=d->m_ar.begin(),end=d->m_ar.end(); itr != end; ++itr)
	{
		itr->m_print( out, itr->m_name.c_str(), &value, itr->m_ofs, indent+1);
	}
	print_indent( out, indent);
	out << "}" << std::endl;
}

/// \brief prints a all elements of a vector
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::vector_&, unsigned int indent)
{
	typename T::const_iterator itr,end;
	for (itr=value.begin(), end = value.end(); itr != end; ++itr)
	{
		printElement( out, name, &*itr, indent);
	}
}

/// \brief prints an atomic element
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::atom_&, unsigned int indent)
{
	print_indent( out, indent);
	out << name << " = '" << value << "'" << std::endl;
}

/// \brief prints a bool element
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::bool_&, unsigned int indent)
{
	print_indent( out, indent);
	out << name << " = '" << std::string(value?"true":"false") << "'" << std::endl;
}

/// \brief prints a log::LogLevel::Level element
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::loglevel_&, unsigned int indent)
{
	print_indent( out, indent);
	out << name << " = '" << value << "'" << std::endl;
}

/// \brief prints a log::SyslogFacility::Facility element
template <typename T>
static void printElement_( std::ostream& out, const char* name, const T& value, const traits::syslogfacility_&, unsigned int indent)
{
	print_indent( out, indent);
	out << name << " = '" << value << "'" << std::endl;
}

/// \brief stub for printing a pointer
template <typename T>
static void printElement_( std::ostream& out, const char* name, T&, const traits::pointer_&, unsigned int indent)
{
	print_indent( out, indent);
	out << name << " = ?" << std::endl;
}

/// \brief the unified print function
template <typename T>
static void printElement( std::ostream& out, const char* name, const T& value, unsigned int indent)
{
	printElement_( out, name, value, traits::getCategory(value), indent);
}

#if 0
/// \brief returns true, if the structure T matches to name
template <typename T>
static bool matchesElement_( const char* name, const T& value, const traits::struct_&)
{
	const DescriptionBase* d = value.description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;

	for (itr=d->m_ar.begin(),end=d->m_ar.end(); itr != end; ++itr)
	{
		if (strcmp( itr->m_type.c_str(), "std::string") == 0)
		{
			const std::string* p = (const std::string*)(const void*)((const char*)(&value) + itr->m_ofs);
			return (strcmp( name, p->c_str()) == 0);
		}
	}
}
#endif

}}// end namespace
#endif

