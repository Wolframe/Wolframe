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
///\file modules/filter/token/mod_filter_token.cpp
///\brief Module for token XML filters
#include "modules/filter/template/filterBuilder.hpp"
#include "filter/token_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

DECLARE_FILTER_OBJECT("token",		TokenFilterObject,		langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UTF-8",	TokenFilterObject_UTF8,		langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UTF-16BE",	TokenFilterObject_UTF16BE,	langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UTF-16LE",	TokenFilterObject_UTF16LE,	langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UCS-2BE",	TokenFilterObject_UCS2BE,	langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UCS-2LE",	TokenFilterObject_UCS2LE,	langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UCS-4BE",	TokenFilterObject_UCS4BE,	langbind::createTokenFilter)
DECLARE_FILTER_OBJECT("token:UCS-4LE",	TokenFilterObject_UCS4LE,	langbind::createTokenFilter)

enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	TokenFilterObject::constructor,
	TokenFilterObject_UTF8::constructor,
	TokenFilterObject_UTF16BE::constructor,
	TokenFilterObject_UTF16LE::constructor,
	TokenFilterObject_UCS2BE::constructor,
	TokenFilterObject_UCS2LE::constructor,
	TokenFilterObject_UCS4BE::constructor,
	TokenFilterObject_UCS4LE::constructor
};


WOLFRAME_DLLEXPORT ModuleEntryPoint entryPoint( 0, "token filter", setModuleLogger, 0, 0, NofObjects, objdef);
