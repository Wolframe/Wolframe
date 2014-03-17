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
///\file appDevel.hpp
///\brief Macros and templates for building Wolframe extensions. This is the main include file for writing Wolframe application extensions in native C++.
#ifndef _Wolframe_APPDEVEL_MODULE_HPP_INCLUDED
#define _Wolframe_APPDEVEL_MODULE_HPP_INCLUDED
#include "appdevel/cppFormFunctionModuleMacros.hpp"	//< modules implementing a set of form functions written in C++
#include "appdevel/normalizeModuleMacros.hpp"		//< modules implementing a set of basic normalizer functions
#include "appdevel/customDatatypeModuleMacros.hpp"	//< modules implementing a set of custom data types
#include "appdevel/runtimeEnvironmentModuleMacros.hpp"	//< modules implementing a program type needing a configurable runtime environment
#include "appdevel/commandHandlerModuleMacros.hpp"	//< modules implementing a command handler and optionally also a program type without configuration
#include "appdevel/programTypeModuleMacros.hpp"		//< modules implementing a program type without configuration
#include "appdevel/ddlCompilerModuleMacros.hpp"		//< modules implementing a DDL compiler for forms
#include "appdevel/filterModuleMacros.hpp"		//< modules implementing a filer
#endif

