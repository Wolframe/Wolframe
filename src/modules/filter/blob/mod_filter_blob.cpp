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
///\brief Module for blob filters that forward the data as binary blob as it is

#include "modules/filter/template/filterBuilder.hpp"
#include "filter/blob_filter.hpp"
#include "logger-v1.hpp"
#include <cstring>

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;
namespace lb = _Wolframe::langbind;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

namespace {

struct BlobFilterObject
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "");}
};
struct BlobFilterObject_UTF8
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UTF-8");}
};
struct BlobFilterObject_UTF16BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UTF-16BE");}
};
struct BlobFilterObject_UTF16LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UTF-16LE");}
};
struct BlobFilterObject_UCS2BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UCS-2BE");}
};
struct BlobFilterObject_UCS2LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UCS-2LE");}
};
struct BlobFilterObject_UCS4BE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UCS-4BE");}
};
struct BlobFilterObject_UCS4LE
{
	static SimpleBuilder* builder()
		{return new FilterBuilder<lb::createBlobFilterPtr>("blob", "UCS-4LE");}
};

}//anonymous namespace


enum {NofObjects=8};
static createBuilderFunc objdef[ NofObjects] =
{
	BlobFilterObject::builder,
	BlobFilterObject_UTF8::builder,
	BlobFilterObject_UTF16BE::builder,
	BlobFilterObject_UTF16LE::builder,
	BlobFilterObject_UCS2BE::builder,
	BlobFilterObject_UCS2LE::builder,
	BlobFilterObject_UCS4BE::builder,
	BlobFilterObject_UCS4LE::builder
};

ModuleEntryPoint entryPoint( 0, "blob filter", setModuleLogger, 0, 0, NofObjects, objdef);


