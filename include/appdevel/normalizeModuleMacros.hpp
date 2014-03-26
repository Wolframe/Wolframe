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
///\file appdevel/normalizeModuleMacros.hpp
///\brief Macros for defining normalization and validaton function module
#include "appdevel/module/normalizeFunctionBuilder.hpp"

//\brief Marks the start of the Wolframe C++ form function module after the includes section.
#define NORMALIZER_MODULE(NAME,DESCRIPTION)\
	static const char* _Wolframe__moduleName()\
	{\
		return NAME;\
	}\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	static _Wolframe::types::NormalizeResourceHandle* _Wolframe__createResourceHandle()\
	{\
		return 0;\
	}\
	static _Wolframe::module::NormalizeFunctionDef _Wolframe__normalizeFunctions[] =\
	{

//\brief Same as NORMALIZER_MODULE but including a singleton resource class (RESOURCECLASS)
#define NORMALIZER_MODULE_WITH_RESOURCE(NAME,DESCRIPTION,RESOURCECLASS)\
	static const char* _Wolframe__moduleName()\
	{\
		return NAME;\
	}\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	static _Wolframe::types::NormalizeResourceHandle* _Wolframe__createResourceHandle()\
	{\
		return new RESOURCECLASS();\
	}\
	static _Wolframe::module::NormalizeFunctionDef _Wolframe__normalizeFunctions[] =\
	{


//\brief Defines normalization function in the NORMALIZER_MODULE section
#define NORMALIZER_FUNCTION(NAME,CONSTRUCTOR)\
		{NAME,&CONSTRUCTOR},\

//\brief Defines the end of the NORMALIZER_MODULE section
#define NORMALIZER_MODULE_END\
		{0,0}\
	};\
	namespace {\
	struct ModuleImpl\
	{\
		static _Wolframe::module::SimpleBuilder* constructor()\
		{\
			return new _Wolframe::module::NormalizeFunctionBuilder( _Wolframe__moduleName(), _Wolframe__normalizeFunctions, &_Wolframe__createResourceHandle);\
		}\
	};\
	}\
	enum {NofObjects=1};\
	static _Wolframe::module::createBuilderFunc _Wolframe__objdef[ NofObjects] =\
	{\
		ModuleImpl::constructor\
	};\
	extern "C" { \
		 _Wolframe::module::ModuleEntryPoint entryPoint( 0, _Wolframe__moduleDescription(), 0, 0, NofObjects, _Wolframe__objdef); \
	}

