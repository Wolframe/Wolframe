/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file mod_employee_assignment_convert.cpp
///\brief Module for testing form functions
#include "module/builtInFunctionBuilder.hpp"
#include "employee_assignment_convert.hpp"
#include "logger-v1.hpp"

_Wolframe::log::LogBackend* logBackendPtr;

using namespace _Wolframe;
using namespace _Wolframe::module;
using namespace _Wolframe::test;

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend*>( logger);
}

namespace {
struct employee_assingment_convert_func
{
	static SimpleBuilder* constructor()
	{
		static const serialize::StructDescriptionBase* param = AssignmentListDoc::getStructDescription();
		static const serialize::StructDescriptionBase* result = AssignmentListDoc::getStructDescription();
		langbind::BuiltInFunction func( convertAssignmentListDoc, param, result);

		return new BuiltInFunctionBuilder( "employee_assignment_convert", func);
	}
};
}

enum {NofObjects=1};
static createBuilderFunc objdef[ NofObjects] =
{
	employee_assingment_convert_func::constructor
};

ModuleEntryPoint entryPoint( 0, "test form function", setModuleLogger, 0, 0, NofObjects, objdef);


