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
///\file textwolf_filterBase.cpp
///\brief Implementations of base functions and classes for filters based on textwolf
#include "filter/textwolf_filterBase.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace langbind;

static std::string getname( const char* name)
{
	std::string val;
	std::size_t kk;
	for (kk=0; name[kk]; kk++)
	{
		if (name[kk] <= ' ') continue;
		if (name[kk] == '-') continue;
		if (name[kk] == ' ') continue;
		val.push_back( name[kk]);
	}
	return val;
}

static bool equalIdent( const std::string& a1, const char* a2)
{
	return boost::algorithm::iequals( a1, a2);
}

const char* TextwolfEncoding::getName( TextwolfEncoding::Id e)
{
	static const char* ar[] = {"","ISO-8859-1", "UTF8", "UTF16", "UTF16BE", "UTF16LE", "UCS2BE", "UCS2LE", "UCS4BE", "UCS4LE"};
	return ar[ (int)e];
}

TextwolfEncoding::Id TextwolfEncoding::getId( const char* name)
{
	std::string nm( getname( name));
	if (equalIdent( nm, "isolatin1")) return IsoLatin;
	if (equalIdent( nm, "iso88591")) return IsoLatin;
	if (equalIdent( nm, "UTF8")) return UTF8;
	if (equalIdent( nm, "UTF16")) return UTF16BE;
	if (equalIdent( nm, "UTF16LE")) return UTF16LE;
	if (equalIdent( nm, "UTF16BE")) return UTF16BE;
	if (equalIdent( nm, "UCS2LE")) return UCS2LE;
	if (equalIdent( nm, "UCS2BE")) return UCS2BE;
	if (equalIdent( nm, "UCS2")) return UCS2BE;
	if (equalIdent( nm, "UCS4LE")) return UCS4LE;
	if (equalIdent( nm, "UCS4BE")) return UCS4BE;
	if (equalIdent( nm, "UCS4")) return UCS4BE;
	return Unknown;
}

