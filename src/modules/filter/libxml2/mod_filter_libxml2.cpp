/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file mod_filter_libxml2.cpp
///\brief Module for libxml2 XML filters
#include "module/filterBuilder.hpp"
#include "module/programTypeBuilder.hpp"
#include "libxml2_filter.hpp"
#if WITH_LIBXSLT
#include "xsltProgramType.hpp"
#endif
#include "logger-v1.hpp"
#include <cstring>

using namespace _Wolframe;
using namespace _Wolframe::module;
namespace lb = _Wolframe::langbind;

namespace {
struct Libxml2FilterObject
{
	static SimpleBuilder* xmlfilter_builder()
		{return new FilterBuilder( "Libxml2Filter", "libxml2", lb::createLibxml2FilterType);}
#if WITH_LIBXSLT
	static SimpleBuilder* xsltfilter_builder()
		{return new ProgramTypeBuilder( "XsltProgramType", "xslt", langbind::createXsltProgramType);}
#endif
};
}//anonymous namespace


#if WITH_LIBXSLT
enum {NofObjects=2};
static createBuilderFunc objdef[ NofObjects] =
{
	Libxml2FilterObject::xmlfilter_builder,
	Libxml2FilterObject::xsltfilter_builder
};
#else
enum {NofObjects=1};
static createBuilderFunc objdef[ NofObjects] =
{
	Libxml2FilterObject::xmlfilter_builder
};
#endif

ModuleEntryPoint entryPoint( 0, "libxml2 XML filter", 0, 0, NofObjects, objdef);


