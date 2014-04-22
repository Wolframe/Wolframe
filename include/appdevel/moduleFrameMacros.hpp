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
///\file appdevel/customDataTypeModuleMacros.hpp
///\brief Macros for defining a custom datatype module
#include "appdevel/module/customDataTypeBuilder.hpp"

///\brief Marks the start of the Wolframe C++ custom datatype module after the includes section.
#define WF_MODULE_BEGIN(NAME,DESCRIPTION)\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	namespace {\
	struct CreateBuilderArray\
	{\
		enum {MaxNofBuilders=64};\
		_Wolframe::module::createBuilderFunc ar[ MaxNofBuilders];\
		std::size_t size;\
		CreateBuilderArray()\
			:size(0)\
		{\
			ar[0] = 0;\
		}\
		CreateBuilderArray operator()( _Wolframe::module::createBuilderFunc func)\
		{\
			if (size +1 >= MaxNofBuilders) throw std::logic_error("too many builder objects defined in module '" #NAME "' (maximum of 64 objects)");\
			ar[ size] = func;\
			ar[ size+1] = 0;\
			size += 1;\
			return *this;\
		}\
	};\
	struct CreateBuilderArrayImpl :public CreateBuilderArray\
	{\
		CreateBuilderArrayImpl()\
		{

#define WF_MODULE_END\
		}\
	};\
	}\
	static CreateBuilderArrayImpl createBuilderArray;\
	extern "C" { \
		_Wolframe::module::ModuleEntryPoint entryPoint( 0, _Wolframe__moduleDescription(), createBuilderArray.ar); \
	}

