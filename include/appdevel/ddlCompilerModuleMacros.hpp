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
///\file appdevel/ddlCompilerModuleMacros.hpp
///\brief Macros for a module implementing a DDL compiler
#include "module/moduleInterface.hpp"
#include "appdevel/module/ddlcompilerBuilder.hpp"

///\brief Defines a Wolframe DDL compiler module after the includes section.
#define DDLCOMPILER_MODULE( DESCRIPTION, LANGUAGE, CREATE_COMPILER)\
	static const char* _Wolframe__moduleDescription()\
	{\
		return DESCRIPTION;\
	}\
	static _Wolframe::module::BaseBuilder* createCompiler()\
	{\
		return new _Wolframe::module::DDLCompilerBuilder( #LANGUAGE "Compiler", #LANGUAGE, CREATE_COMPILER);\
	}\
	static _Wolframe::module::BaseBuilder* (*builder[])() =\
	{\
		createCompiler, NULL\
	};\
	extern "C" {\
		_Wolframe::module::ModuleEntryPoint \
		entryPoint( 0, _Wolframe__moduleDescription(), builder );\
	}

