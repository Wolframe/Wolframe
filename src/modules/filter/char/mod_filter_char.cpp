/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file modules/filter/char/mod_filter_char.cpp
///\brief Module for char XML filters
#include "modules/filter/template/filterObjectBuilder.hpp"
#include "filter/char_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

DECLARE_FILTER_OBJECT("char",		CharFilterObject,		langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UTF-8",	CharFilterObject_UTF8,		langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UTF-16BE",	CharFilterObject_UTF16BE,	langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UTF-16LE",	CharFilterObject_UTF16LE,	langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UCS-2BE",	CharFilterObject_UCS2BE,	langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UCS-2LE",	CharFilterObject_UCS2LE,	langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UCS-4BE",	CharFilterObject_UCS4BE,	langbind::createCharFilter)
DECLARE_FILTER_OBJECT("char:UCS-4LE",	CharFilterObject_UCS4LE,	langbind::createCharFilter)

enum {NofObjects=8};
static createObjectFunc objdef[ NofObjects] =
{
	CharFilterObject::constructor,
	CharFilterObject_UTF8::constructor,
	CharFilterObject_UTF16BE::constructor,
	CharFilterObject_UTF16LE::constructor,
	CharFilterObject_UCS2BE::constructor,
	CharFilterObject_UCS2LE::constructor,
	CharFilterObject_UCS4BE::constructor,
	CharFilterObject_UCS4LE::constructor
};

ModuleEntryPoint entryPoint( 0, "char filter", setModuleLogger, 0, 0, NofObjects, objdef);


