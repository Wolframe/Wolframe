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
///\file modules/filter/libxml2/mod_filter_libxml2.cpp
///\brief Module for libxml2 XML filters
#include "modules/filter/template/filterContainerBuilder.hpp"
#include "filter/libxml2_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

DECLARE_FILTER_OBJECT("xml:libxml2",		Libxml2FilterObject,		langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UTF-8",	Libxml2FilterObject_UTF8,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UTF-16BE",	Libxml2FilterObject_UTF16BE,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UTF-16LE",	Libxml2FilterObject_UTF16LE,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UCS-2BE",	Libxml2FilterObject_UCS2BE,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UCS-2LE",	Libxml2FilterObject_UCS2LE,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UCS-4BE",	Libxml2FilterObject_UCS4BE,	langbind::createLibxml2Filter)
DECLARE_FILTER_OBJECT("xml:libxml2:UCS-4LE",	Libxml2FilterObject_UCS4LE,	langbind::createLibxml2Filter)

enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	Libxml2FilterObject::constructor,
	Libxml2FilterObject_UTF8::constructor,
	Libxml2FilterObject_UTF16BE::constructor,
	Libxml2FilterObject_UTF16LE::constructor,
	Libxml2FilterObject_UCS2BE::constructor,
	Libxml2FilterObject_UCS2LE::constructor,
	Libxml2FilterObject_UCS4BE::constructor,
	Libxml2FilterObject_UCS4LE::constructor
};

#ifndef _WIN32
#define DLLEXPORT 
#else
#define DLLEXPORT __declspec( dllexport )
#endif

extern "C" DLLEXPORT ModuleEntryPoint entryPoint( 0, "libxml2 XML filter", setModuleLogger, 0, 0, NofObjects, objdef);


