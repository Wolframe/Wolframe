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
///\file appdevel/cppFormFunctionModuleMacros.hpp
///\brief Macros and templates for building C++ an application form function module

#include "appdevel/module/cppFormFunctionTemplate.hpp"
#include "appdevel/module/cppFormFunctionBuilder.hpp"

//\brief Marks the start if the Wolframe C++ form function module after the includes section.
#define CPP_APPLICATION_FORM_FUNCTION_MODULE(NAME)\
	static const char* _Wolframe__moduleName()\
	{\
	return NAME;\
	}\
	static _Wolframe::module::CppFormFunctionDef _Wolframe__cppFormFunctions[] =\
	{

#define CPP_APPLICATION_FORM_FUNCTION(NAME,FUNCTION,OUTPUT,INPUT)\
	{NAME,appdevel::CppFormFunction<INPUT,OUTPUT,FUNCTION>::declaration()},

//\brief Marks the end if the Wolframe C++ form function module.
#define CPP_APPLICATION_FORM_FUNCTION_MODULE_END\
	{0,langbind::CppFormFunction()}\
	};\
	namespace {\
	struct ModuleImpl\
	{\
	static _Wolframe::module::SimpleBuilder* constructor()\
	{\
		return new _Wolframe::module::CppFormFunctionBuilder( _Wolframe__moduleName(), _Wolframe__cppFormFunctions);\
	}\
	};\
	}\
	enum {NofObjects=1};\
	static _Wolframe::module::createBuilderFunc _Wolframe__objdef[ NofObjects] =\
	{\
		ModuleImpl::constructor\
	};\
	extern "C" { \
		 module::ModuleEntryPoint entryPoint( 0, _Wolframe__moduleName(), 0, 0, NofObjects, _Wolframe__objdef); \
	}

