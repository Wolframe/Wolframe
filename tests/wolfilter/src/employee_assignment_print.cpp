/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.
patrick@toimup:~/Projects/github/Wolframe$ git diff
diff --git a/tests/testProtocolLFdotEscaping.cpp b/tests/testProtocolLFdotEscaping.cpp
index c7e31de..75de46d 100644
-- a/tests/testProtocolLFdotEscaping.cpp
+++ b/tests/testProtocolLFdotEscaping.cpp
@@ -126,4 +126,3 @@ int main( int argc, char **argv )
	return RUN_ALL_TESTS();
 }


diff --git a/tests/wolfilter/src/employee_assignment_print.cpp b/tests/wolfilter/src/employee_assignment_print.cpp
deleted file mode 100644
index 64713d9..0000000
-- a/tests/wolfilter/src/employee_assignment_print.cpp
+++ /dev/null
@@ -1,103 +0,0 @@
/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
///\file wolfilter/src/employee_assignment_print.cpp
///\brief Implementation wolfilter test plugin function

#include "serialize/struct/mapDescription.hpp"
#include "wolfilter/src/employee_assignment_print.hpp"

using namespace _Wolframe;
using namespace test;

serialize::MapDescriptionBase* Task::getMapDescription()
{
	struct ThisDescription :public serialize::MapDescription<Task>
	{
		ThisDescription()
		{
			(*this)
			("key", &Task::key)
			("title", &Task::title)
			("customernumber", &Task::customernumber);
		}
	};
	static ThisDescription rt;
	return &rt;
}

serialize::MapDescriptionBase* Employee::getMapDescription()
{
	struct ThisDescription :public serialize::MapDescription<Employee>
	{
		ThisDescription()
		{
			(*this)
			("firstname", &Employee::firstname)
			("surname", &Employee::surname)
			("phone", &Employee::phone);
		}
	};
	static ThisDescription rt;
	return &rt;
}

serialize::MapDescriptionBase* Assignment::getMapDescription()
{
	struct ThisDescription :public serialize::MapDescription<Assignment>
	{
		ThisDescription()
		{
			(*this)
			("issuedate", &Assignment::issuedate)
			("employee", &Assignment::employee)
			("task", &Assignment::task);
		}
	};
	static ThisDescription rt;
	return &rt;
}

serialize::MapDescriptionBase* AssignmentList::getMapDescription()
{
	struct ThisDescription :public serialize::MapDescription<AssignmentList>
	{
		ThisDescription()
		{
			(*this)
			("assignment", &AssignmentList::assignment);
		}
	};
	static ThisDescription rt;
	return &rt;
}
