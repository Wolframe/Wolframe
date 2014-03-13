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
///\file employee_assignment_convert.cpp
///\brief Implementation wolfilter test plugin data

#include "serialize/struct/structDescription.hpp"
#include "employee_assignment_convert.hpp"
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace _Wolframe::test;

namespace {
struct TaskDescription :public serialize::StructDescription<Task>
{
	TaskDescription()
	{
		(*this)
		("key", &Task::key)
		("title", &Task::title)
		("customernumber", &Task::customernumber);
	}
};

struct EmployeeDescription :public serialize::StructDescription<Employee>
{
	EmployeeDescription()
	{
		(*this)
		("firstname", &Employee::firstname)
		("surname", &Employee::surname)
		("phone", &Employee::phone);
	}
};

struct AssignmentDescription :public serialize::StructDescription<Assignment>
{
	AssignmentDescription()
	{
		(*this)
		("issuedate", &Assignment::issuedate)
		("employee", &Assignment::employee)
		("task", &Assignment::task);
	}
};

struct AssignmentListDescription :public serialize::StructDescription<AssignmentList>
{
	AssignmentListDescription()
	{
		(*this)
		("assignment", &AssignmentList::assignment);
	}
};

struct AssignmentListDocDescription :public serialize::StructDescription<AssignmentListDoc>
{
	AssignmentListDocDescription()
	{
		(*this)
		("assignmentlist", &AssignmentListDoc::assignmentlist);
	}
};
}


const serialize::StructDescriptionBase* Task::getStructDescription()
{
	static TaskDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* Employee::getStructDescription()
{
	static EmployeeDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* Assignment::getStructDescription()
{
	static AssignmentDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* AssignmentList::getStructDescription()
{
	static AssignmentListDescription rt;
	return &rt;
}

const serialize::StructDescriptionBase* AssignmentListDoc::getStructDescription()
{
	static AssignmentListDocDescription rt;
	return &rt;
}

