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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file modules/filter/textwolf/mod_filter_textwolf.cpp
///\brief Module for textwolf XML filters
#include "modules/filter/template/filterObjectBuilder.hpp"
#include "filter/textwolf_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

DECLARE_FILTER_OBJECT("xml:textwolf",		TextwolfXMLFilterObject,	langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UTF-8",	TextwolfXMLFilterObject_UTF8,	langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UTF-16BE",	TextwolfXMLFilterObject_UTF16BE,langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UTF-16LE",	TextwolfXMLFilterObject_UTF16LE,langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UCS-2BE",	TextwolfXMLFilterObject_UCS2BE,	langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UCS-2LE",	TextwolfXMLFilterObject_UCS2LE,	langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UCS-4BE",	TextwolfXMLFilterObject_UCS4BE,	langbind::createTextwolfXmlFilter)
DECLARE_FILTER_OBJECT("xml:textwolf:UCS-4LE",	TextwolfXMLFilterObject_UCS4LE,	langbind::createTextwolfXmlFilter)

enum {NofObjects=8};
static createObjectFunc objdef[ NofObjects] =
{
	TextwolfXMLFilterObject::constructor,
	TextwolfXMLFilterObject_UTF8::constructor,
	TextwolfXMLFilterObject_UTF16BE::constructor,
	TextwolfXMLFilterObject_UTF16LE::constructor,
	TextwolfXMLFilterObject_UCS2BE::constructor,
	TextwolfXMLFilterObject_UCS2LE::constructor,
	TextwolfXMLFilterObject_UCS4BE::constructor,
	TextwolfXMLFilterObject_UCS4LE::constructor
};

ModuleEntryPoint entryPoint( 0, "textwolf XML filter", setModuleLogger, 0, 0, NofObjects, objdef);


