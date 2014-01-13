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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file modules/normalize/locale/mod_normalize_locale.hpp
///\brief Module for normalization functions based on boost locale (ICU)
#include "module/normalizeFunctionBuilder.hpp"
#include "logger-v1.hpp"
#include "localeNormalize.hpp"

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

static langbind::ResourceHandle* createLocaleResourceHandle()
{
	return new langbind::LocaleResourceHandle();
}

static NormalizeFunctionDef normalizeFunctions[] =
{
	{"tolower", &langbind::create_tolower_NormalizeFunction},
	{"toupper", &langbind::create_toupper_NormalizeFunction},
	{"totitle", &langbind::create_totitle_NormalizeFunction},
	{"foldcase", &langbind::create_foldcase_NormalizeFunction},
	{"nfd", &langbind::create_nfd_NormalizeFunction},
	{"nfc", &langbind::create_nfc_NormalizeFunction},
	{"nfkd", &langbind::create_nfkd_NormalizeFunction},
	{"nfkc", &langbind::create_nfkc_NormalizeFunction},
	{"latinword", &langbind::create_latinword_NormalizeFunction},
	{"ascii_de", &langbind::create_ascii_de_NormalizeFunction},
	{"ascii_eu", &langbind::create_ascii_eu_NormalizeFunction},
	{0,0}
};

namespace {
struct NormalizeProcessor
{
	static SimpleBuilder* constructor()
	{
		return new NormalizeFunctionBuilder( "BoostLocaleNormalizer", "localeconv", normalizeFunctions, &createLocaleResourceHandle);
	}
};
}//anonymous namespace

enum {NofObjects=1};
static createBuilderFunc objdef[ NofObjects] =
{
	NormalizeProcessor::constructor
};

ModuleEntryPoint entryPoint( 0, "normalize character conversion functions", setModuleLogger, 0, 0, NofObjects, objdef);

