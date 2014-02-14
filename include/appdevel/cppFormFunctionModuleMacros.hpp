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
#include "appdevel/cppFormFunctionTemplate.hpp"
#include "logger-v1.hpp"

//\brief Marks the start if the Wolframe C++ form function module after the includes section.
#define CPP_APPLICATION_FORM_FUNCTION_MODULE(NAME)\
	_Wolframe::log::LogBackend* logBackendPtr;\
	\
	static void setModuleLogger( void* logger )\
	{\
		logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);\
	}\
	\
	static const char* _Wolframe__moduleName()\
	{\
	return NAME;\
	}\

//\brief Marks the end if the Wolframe C++ form function module.
//\param[in] NofObjects Number of functions to export from the module
//\param[in] Objects Array of function declarations to export from the module
#define CPP_APPLICATION_FORM_FUNCTION_MODULE_END( NofObjects, Objects)\
	module::ModuleEntryPoint entryPoint( 0, _Wolframe__moduleName(), setModuleLogger, 0, 0, NofObjects, Objects);

