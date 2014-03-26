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
///\file mod_normalize_locale.hpp
///\brief Module for normalization functions based on boost locale (ICU)
#include "appdevel/normalizeModuleMacros.hpp"
#include "logger-v1.hpp"
#include "localeNormalize.hpp"

using namespace _Wolframe::langbind;

NORMALIZER_MODULE_WITH_RESOURCE( "Base64Normalizer", "base 64 encoding/decoding as normalization functions", LocaleResourceHandle)
NORMALIZER_FUNCTION(	"tolower",	create_tolower_NormalizeFunction)
NORMALIZER_FUNCTION(	"toupper",	create_toupper_NormalizeFunction)
NORMALIZER_FUNCTION(	"totitle",	create_totitle_NormalizeFunction)
NORMALIZER_FUNCTION(	"foldcase",	create_foldcase_NormalizeFunction)
NORMALIZER_FUNCTION(	"conv_nfd",	create_nfd_NormalizeFunction)
NORMALIZER_FUNCTION(	"conv_nfc",	create_nfc_NormalizeFunction)
NORMALIZER_FUNCTION(	"conv_nfkd",	create_nfkd_NormalizeFunction)
NORMALIZER_FUNCTION(	"conv_nfkc",	create_nfkc_NormalizeFunction)
NORMALIZER_FUNCTION(	"latinword",	create_latinword_NormalizeFunction)
NORMALIZER_FUNCTION(	"ascii_de",	create_ascii_de_NormalizeFunction)
NORMALIZER_FUNCTION(	"ascii_eu",	create_ascii_eu_NormalizeFunction)
NORMALIZER_MODULE_END
