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
/// \file appdevel/cppFormFunctionModuleMacros.hpp
/// \brief Macros and templates for building C++ an application form function module

#include "appdevel/module/cppFormFunctionTemplate.hpp"
#include "appdevel/module/cppFormFunctionBuilder.hpp"
#include "appdevel/module/cppFormFunctionTemplate.hpp"

/// \brief Defines normalization function
#define WF_FORM_FUNCTION(NAME,FUNCTION,OUTPUT,INPUT)\
{\
	struct Constructor\
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			_Wolframe::serialize::CppFormFunction func = _Wolframe::appdevel::CppFormFunction<OUTPUT,INPUT,FUNCTION>::declaration();\
			return new _Wolframe::module::CppFormFunctionBuilder( "CppFormFunction_" NAME, NAME, func);\
		}\
	};\
	(*this)(&Constructor ::impl);\
}

/// \brief Defines normalization function without return value (empty result)
#define WF_FORM_PROCEDURE(NAME,PROCEDURE,INPUT)\
{\
	struct Constructor\
	{\
		static _Wolframe::module::BuilderBase* impl()\
		{\
			_Wolframe::serialize::CppFormFunction func = _Wolframe::appdevel::CppFormFunction<_Wolframe::serialize::EmptyStruct,INPUT,PROCEDURE>::declaration();\
			return new _Wolframe::module::CppFormFunctionBuilder( "CppFormFunction_" NAME, NAME, func);\
		}\
	};\
	(*this)(&Constructor ::impl);\
}
