/*
---------------------------------------------------------------------
    The template library textwolf implements an input iterator on
    a set of XML path expressions without backward references on an
    STL conforming input iterator as source. It does no buffering
    or read ahead and is dedicated for stream processing of XML
    for a small set of XML queries.
    Stream processing in this context refers to processing the
    document without buffering anything but the current result token
    processed with its tag hierarchy information.

    Copyright (C) 2010,2011,2012,2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3.0 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of textwolf can be found at 'http://github.com/patrickfrey/textwolf'
	For documentation see 'http://patrickfrey.github.com/textwolf'

--------------------------------------------------------------------
*/
#ifndef __TEXTWOLF_TRAITS_HPP__
#define __TEXTWOLF_TRAITS_HPP__
/// \file textwolf/traits.hpp
/// \brief Type traits

namespace textwolf {
namespace traits {

/// \class TypeCheck
/// \brief Test structure to stear the compiler
class TypeCheck
{
public:
	struct YES {};
	struct NO {};

	template<typename T, typename U>
	struct is_same 
	{
		static const NO type() {return NO();}
	};
	
	template<typename T>
	struct is_same<T,T>
	{
		static const YES type() {return YES();} 
	};
};

}}//namespace
#endif
