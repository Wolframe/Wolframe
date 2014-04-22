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
#include "appdevel/moduleFrameMacros.hpp"
#include "logger-v1.hpp"
#include "localeNormalize.hpp"

using namespace _Wolframe::langbind;

WF_MODULE_BEGIN( "Base64Normalizer", "base 64 encoding/decoding as normalization functions")
 WF_NORMALIZER_RESOURCE( LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "tolower", create_tolower_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "toupper", create_toupper_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "totitle", create_totitle_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "foldcase", create_foldcase_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "conv_nfd", create_nfd_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "conv_nfc", create_nfc_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "conv_nfkd", create_nfkd_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "conv_nfkc", create_nfkc_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "latinword", create_latinword_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "ascii_de", create_ascii_de_NormalizeFunction, LocaleResourceHandle)
 WF_NORMALIZER_WITH_RESOURCE( "ascii_eu", create_ascii_eu_NormalizeFunction, LocaleResourceHandle)
WF_MODULE_END
