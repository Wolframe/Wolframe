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
///\file wolfilter/modules/employee_assignment_convert/employee_assignment_convert.cpp
///\brief Implementation wolfilter test plugin function

#include "serialize/struct/filtermapDescription.hpp"
#include "wolfilter/modules/employee_assignment_convert/employee_assignment_convert.hpp"
#include <string>
#include <vector>

using namespace _Wolframe;
using namespace test;

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

static void convertString( std::string& res, const std::string& param)
{
	std::string::const_iterator itr=param.begin();
	while (itr != param.end())
	{
		if (*itr >= '0' && *itr <= '9') res.push_back( '9'-*itr + '0');
		else if (*itr >= 'a' && *itr <= 'z') {res.push_back( *itr); res.push_back( *itr);}
		else if (*itr >= 'A' && *itr <= 'Z') {res.push_back( ::tolower(*itr));}
		else if (*itr == ' ' || *itr == '-' || *itr == '_') {res.push_back( *itr);}
		++itr;
	}
}

int AssignmentListDoc::convert( AssignmentListDoc& res, const AssignmentListDoc& param)
{
	std::vector<Assignment>::const_iterator itr=param.assignmentlist.assignment.begin();
	while (itr != param.assignmentlist.assignment.end())
	{
		Assignment aa;
		convertString( aa.issuedate, itr->issuedate);
		convertString( aa.employee.firstname, itr->employee.firstname);
		convertString( aa.employee.surname, itr->employee.surname);
		convertString( aa.employee.phone, itr->employee.phone);
		std::vector<Task>::const_iterator taskitr=itr->task.begin();
		while (taskitr != itr->task.end())
		{
			Task tt;
			convertString( tt.title, taskitr->title);
			convertString( tt.key, taskitr->key);
			tt.customernumber = taskitr->customernumber + 1;
			aa.task.push_back( tt);
			++taskitr;
		}
		res.assignmentlist.assignment.push_back( aa);
		++itr;
	}
	return 0;
}


int _Wolframe::test::convertAssignmentListDoc( void* res, const void* param)
{
	return AssignmentListDoc::convert( *(AssignmentListDoc*)res, *(const AssignmentListDoc*) param);
}

