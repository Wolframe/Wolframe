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
///\file modules/filter/line/mod_filter_line.cpp
///\brief Module for line XML filters
#include "modules/filter/template/filterBuilder.hpp"
#include "filter/line_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

DECLARE_FILTER_OBJECT("line",		LineFilterObject,		langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UTF-8",	LineFilterObject_UTF8,		langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UTF-16BE",	LineFilterObject_UTF16BE,	langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UTF-16LE",	LineFilterObject_UTF16LE,	langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UCS-2BE",	LineFilterObject_UCS2BE,	langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UCS-2LE",	LineFilterObject_UCS2LE,	langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UCS-4BE",	LineFilterObject_UCS4BE,	langbind::createLineFilter)
DECLARE_FILTER_OBJECT("line:UCS-4LE",	LineFilterObject_UCS4LE,	langbind::createLineFilter)

enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	LineFilterObject::constructor,
	LineFilterObject_UTF8::constructor,
	LineFilterObject_UTF16BE::constructor,
	LineFilterObject_UTF16LE::constructor,
	LineFilterObject_UCS2BE::constructor,
	LineFilterObject_UCS2LE::constructor,
	LineFilterObject_UCS4BE::constructor,
	LineFilterObject_UCS4LE::constructor
};

ModuleEntryPoint entryPoint( 0, "line filter", setModuleLogger, 0, 0, NofObjects, objdef);
