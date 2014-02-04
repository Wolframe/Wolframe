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
#ifndef _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#define _Wolframe_COM_AUTOMATION_TYPELIB_HPP_INCLUDED
#include "langbind/formFunction.hpp"
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include <stdexcept>
#include <boost/shared_ptr.hpp>

struct IRecordInfo;
struct ITypeInfo;
struct ITypeLib;
struct tagVARIANT;
struct tagTLIBATTR;

namespace _Wolframe {
namespace comauto {

class TypeLib
{
public:
	explicit TypeLib( const std::string& file);
	TypeLib( const TypeLib& o)	:m_impl(o.m_impl){}

	void print( std::ostream& out) const;
	void printvalue( std::ostream& out, const std::string& name, const tagVARIANT& val, const ITypeInfo* typeinfo, std::size_t indentcnt=0) const;

	const IRecordInfo* getRecordInfo( const ITypeInfo* typeinfo) const;

	const ITypeLib* typelib() const;
	const tagTLIBATTR* libattr() const;
	ITypeInfo* getProviderInterface() const;

	class Impl;

private:
	boost::shared_ptr<Impl> m_impl;
};

}} //namespace
#endif

