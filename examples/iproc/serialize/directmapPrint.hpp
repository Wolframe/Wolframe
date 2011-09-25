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
///\file serialize/directmapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the direct map

#ifndef _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#include "serialize/directmapTraits.hpp"
#include "serialize/directmapBase.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace serialize {

static void printElem( protocol::FormatOutput::ElementType tp, const void* elem, std::size_t elemsize, protocol::FormatOutput*& out, std::string& buf)
{
	if (!out->print( tp, elem, elemsize))
	{
		buf.append( out->charptr(), out->pos());
		out->release();
		if (!out->print( tp, elem, elemsize))
		{
			protocol::FormatOutput* ff = out->createFollow();
			if (ff)
			{
				delete out;
				out = ff;
			}
			if (!out->print( tp, elem, elemsize))
			{
				throw std::runtime_error( "buffer of format output too small to hold one element");
			}
		}
	}
}

template <typename T>
static void printObject( const char* tag, const T& obj, protocol::FormatOutput*& out, std::string& buf);


template <typename T>
void print_( const char* tag, const void* obj, const struct_&, protocol::FormatOutput*& out, std::string& buf)
{
	static const DescriptionBase* descr = T::getDescription();
	bool isContent = true;

	if (tag) printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, buf);

	DescriptionBase::Map::const_iterator itr = descr->m_elem.begin(),end = descr->m_elem.end();
	for (;itr != end; ++itr)
	{
		if (itr->second.m_isAtomic() && !isContent)
		{
			printElem( protocol::FormatOutput::Attribute, itr->first.c_str(), itr->first.size(), out, buf);
			itr->second.m_print( 0, (char*)obj+itr->second.m_ofs, out, buf);
		}
		else
		{
			isContent = true;
			itr->second.m_print( itr->first.c_str(), (char*)obj+itr->second.m_ofs, out, buf);
		}
	}
	if (tag) printElem( protocol::FormatOutput::CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, const void* obj, const arithmetic_&, protocol::FormatOutput*& out, std::string& buf)
{
	if (tag) printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, buf);
	std::string value( boost::lexical_cast<std::string>( *((T*)obj)));
	printElem( protocol::FormatOutput::Value, value.c_str(), value.size(), out, buf);
	if (tag) printElem( protocol::FormatOutput::CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, const void* obj, const bool_&, protocol::FormatOutput*& out, std::string& buf)
{
	if (tag) printElem( protocol::FormatOutput::OpenTag, tag, std::strlen(tag), out, buf);
	printElem( protocol::FormatOutput::Value, (*((T*)obj))?"t":"f", 1, out, buf);
	if (tag) printElem( protocol::FormatOutput::CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, const void* obj, const vector_&, protocol::FormatOutput*& out, std::string& buf)
{
	if (!tag)
	{
		throw std::runtime_error( "non printable structure");
	}
	for (typename T::const_iterator itr=((T*)obj)->begin(); itr!=((T*)obj)->end(); itr++)
	{
		printElem( protocol::FormatOutput::OpenTag, tag, strlen(tag), out, buf);
		printObject( 0, *itr, out, buf);
		printElem( protocol::FormatOutput::CloseTag, "", 0, out, buf);
	}
}

template <typename T>
static void printObject( const char* tag, const T& obj, protocol::FormatOutput*& out, std::string& buf)
{
	print_<T>( tag, (void*)&obj, getCategory(obj), out, buf);
}

template <typename T>
struct IntrusivePrinter
{
	static void print( const char* tag, const void* obj, protocol::FormatOutput*& out, std::string& buf)
	{
		print_<T>( tag, obj, getCategory(*(T*)obj), out, buf);
		buf.append( out->charptr(), out->pos());
		out->release();
	}
};

}}//namespace
#endif

