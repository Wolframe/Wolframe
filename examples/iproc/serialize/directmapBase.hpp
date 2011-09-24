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
///\file serialize/directmapBase.hpp
///\brief Defines the non intrusive base class of serialization for the direct map

#ifndef _Wolframe_DIRECTMAP_BASE_HPP_INCLUDED
#define _Wolframe_DIRECTMAP_BASE_HPP_INCLUDED
#include "protocol/inputfilter.hpp"
#include "protocol/formatoutput.hpp"
#include <cstddef>
#include <map>

namespace _Wolframe {
namespace serialize {

class ProcessingContext
{
public:
	const char* getLastError() const {return m_lasterror;}
private:
friend class DescriptionBase;
	char m_lasterror[ 256];
	std::string m_content;
};

class DescriptionBase
{
public:
	typedef void (*Parse)( const char* tag, void* obj, protocol::InputFilter& flt);
	typedef void (*Print)( const char* tag, void* obj, protocol::FormatOutput& out, std::string& buf);
	typedef bool (*IsAtomic)();

	DescriptionBase( std::size_t ofs, IsAtomic ia, Parse pa, Print pr)
		:m_ofs(ofs),m_isAtomic(ia),m_parse(pa),m_print(pr){}
	DescriptionBase( const DescriptionBase& o)
		:m_ofs(o.m_ofs),m_elem(o.m_elem),m_isAtomic(o.m_isAtomic),m_parse(o.m_parse),m_print(o.m_print){}

	bool parse( void* obj, protocol::InputFilter& in, ProcessingContext& ctx) const;
	bool print( void* obj, protocol::FormatOutput& out, ProcessingContext& ctx) const;
	bool isAtomic() const		{return m_isAtomic();}
public:
	std::size_t m_ofs;
	std::map<std::string,DescriptionBase> m_elem;
	IsAtomic m_isAtomic;
	Parse m_parse;
	Print m_print;
};

}}//namespace
#endif
