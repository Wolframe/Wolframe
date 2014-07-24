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
/// \file appdevel/normalizeModuleMacros.hpp
/// \brief Macros for defining normalization and validaton function module
#include "appdevel/module/normalizeFunctionBuilder.hpp"
#include "types/normalizeFunction.hpp"
#include "types/variant.hpp"
#include <boost/shared_ptr.hpp>

#define WF_NORMALIZER_RESOURCE(RESOURCECLASS)\
struct macro__WF_NORMALIZER_RESOURCE__ ## RESOURCECLASS\
{\
	static const boost::shared_ptr<RESOURCECLASS>& get()\
	{\
		static boost::shared_ptr<RESOURCECLASS> rt;\
		if (!rt.get())\
		{\
			rt.reset( new RESOURCECLASS());\
		}\
		return rt;\
	}\
};\

/// \brief Defines normalization function
#define WF_NORMALIZER(NAME,NORMALIZERCLASS)\
{\
	struct Constructor\
	{\
		static _Wolframe::types::NormalizeFunction* create( _Wolframe::types::NormalizeResourceHandle*, const std::vector<_Wolframe::types::Variant>& arg)\
		{\
			return new NORMALIZERCLASS( arg);\
		}\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			return new _Wolframe::module::NormalizeFunctionBuilder( "NormalizeFunction_" #NAME, NAME, create);\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

/// \brief Defines normalization function
#define WF_NORMALIZER_WITH_RESOURCE(NAME,NORMALIZERCLASS,RESOURCECLASS)\
{\
	struct Constructor\
	{\
		static _Wolframe::types::NormalizeFunction* create( _Wolframe::types::NormalizeResourceHandle* reshnd, const std::vector<_Wolframe::types::Variant>& arg)\
		{\
			return new NORMALIZERCLASS( reshnd, arg);\
		}\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			return new _Wolframe::module::NormalizeFunctionBuilder( "NormalizeFunction_" #NAME, NAME, create, macro__WF_NORMALIZER_RESOURCE__ ## RESOURCECLASS::get());\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

