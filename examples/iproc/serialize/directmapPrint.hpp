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
///\file serialize/directmapParse.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the direct map

#ifndef _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_PRINT_HPP_INCLUDED
#include "directmapTraits.hpp"
#include <stdexcept>


///the intrusive part of the definitions is put into an anonymous namespace:
namespace {

template<typename N>
static bool isAttributeCatg( N&)
{
	return true;
}
template<>
static bool isAttributeCatg( struct_&)
{
	return false;
}
template<>
static bool isAttributeCatg( vector_&)
{
	return false;
}

static void printElem( protocol::FormatOutput::ElementType tp, void* elem, std::size_t elemsize, protocol::FormatOutput& out, std::string& buf)
{
	if (!out->print( tp, elem, elemsize))
	{
		buf.append( out.charptr(), out.pos());
		out.release();
		if (!out->print( tp, elem, elemsize))
		{
			throw std::runtime_error( "buffer of format output too small to hold one element");
		}
	}
}

template <typename T>
void print_( const char* tag, void* obj, struct_&, protocol::FormatOutput& out, std::string& buf)
{
	static const DescriptionBase* descr = T::description();
	bool isContent = true;

	if (tag) printElem( OpenTag, itr->first.c_str(), itr->first.size(), out, buf);

	std::map<std::string,DescriptionBase>::const_iterator itr = descr->m_elem->begin();
	for (;itr != descr->m_elem->end(); ++itr)
	{
		if (itr->isAtomic() && !isContent)
		{
			printElem( Attribute, itr->first.c_str(), itr->first.size(), out, buf);
			itr->m_print( 0, (char*)obj+itr->m_ofs, out, buf);
		}
		else
		{
			isContent = true;
			itr->print( itr->first.c_str(), (char*)obj+itr->m_ofs, out, buf);
		}
	}
	if (tag) printElem( CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, void* obj, arithmetic_&, protocol::FormatOutput& out, std::string& buf)
{
	if (tag) printElem( OpenTag, itr->first.c_str(), itr->first.size(), out, buf);
	std::string value( boost::lexical_cast<std::string>( *((T*)obj)));
	printElem( Value, value.c_str(), value.size(), out, buf);
	if (tag) printElem( CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, void* obj, bool_&, protocol::FormatOutput& out, std::string& buf)
{
	if (tag) printElem( OpenTag, itr->first.c_str(), itr->first.size(), out, buf);
	printElem( Value, (*((T*)obj))?"t":"f", 1, out, buf);
	if (tag) printElem( CloseTag, "", 0, out, buf);
}

template <typename T>
void print_( const char* tag, void* obj, vector_&, protocol::FormatOutput& out, std::string& buf)
{
	for (T::const_iterator itr=((T*)obj)->begin(); itr!=((T*)obj)->end(); itr++)
	{
		if (tag) printElem( OpenTag, itr->first.c_str(), itr->first.size(), out, buf);
		itr->print( 0, (char*)obj+itr->m_ofs, out, buf);
		if (tag) printElem( CloseTag, "", 0, out, buf);
	}
}

}//anonymous namespace

namespace _Wolframe {
namespace serialize {
namespace dm {

template <typename T>
static void print( const char* tag, T* obj, protocol::FormatOutput& out, std::string& buf)
{
	print_<T>( tag, (void*)obj, getCategory(val), out, buf);
	buf.append( out.charptr(), out.pos());
	out.release();
}

}}}//namespace
#endif

