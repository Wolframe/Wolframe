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
//
// a test module
//

#include "logger-v1.hpp"
#include "common.hpp"

namespace _Wolframe {
namespace module {
namespace test_containers {

#ifdef _WIN32
#define DLLEXPORT __declspec( dllexport )
#else
#define DLLEXPORT
#endif

extern "C" DLLEXPORT ModuleEntryPoint entryPoint;

// the implementation of a plugin full-filling the TestUnit interface
class TestUnitImpl1 : public TestUnit1
{
	virtual bool resolveDB( const db::DatabaseProvider& /* db */ );

public:
	// test unit has no configuration, so have an empty constructor
	TestUnitImpl1( );
	
	virtual ~TestUnitImpl1( );

	virtual const std::string hello( );
};

// the implementation of a plugin full-filling the TestUnit interface
class TestUnitImpl2 : public TestUnit2
{
	virtual bool resolveDB( const db::DatabaseProvider& /* db */ );

public:
	// test unit has no configuration, so have an empty constructor
	TestUnitImpl2( );
	
	virtual ~TestUnitImpl2( );

	virtual const std::string hullo( );
};

}}} // namespace _Wolframe::module::test_containers
