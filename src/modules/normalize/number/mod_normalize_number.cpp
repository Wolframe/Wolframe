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
///\file modules/normalize/number/mod_normalize_number.hpp
///\brief Module for number normalization and validating functions
#include "module/normalizeFunctionBuilder.hpp"
#include "logger-v1.hpp"
#include "numberNormalize.hpp"

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

static langbind::ResourceHandle numberResource;

namespace {
struct NormalizeProcessor
{

	static SimpleBuilder* constructor()
	{
		return new NormalizeFunctionBuilder( "NumberNormalizer", "number", langbind::normalizeFunctions, langbind::createNumberNormalizeFunction, &numberResource);
	}
};
}//anonymous namespace

enum {NofObjects=1};
static createBuilderFunc objdef[ NofObjects] =
{
	NormalizeProcessor::constructor
};

ModuleEntryPoint entryPoint( 0, "normalizers and validators numbers", setModuleLogger, 0, 0, NofObjects, objdef);

