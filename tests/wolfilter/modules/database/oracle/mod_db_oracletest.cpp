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
/// \file  mod_db_oracletest.cpp
/// \brief Module for a database implementation used for tests
///  based on the postgres database module

#include "OracleTest.hpp"
#include "module/moduleInterface.hpp"
#include "logger-v1.hpp"

namespace _Wolframe {
namespace module {

typedef ConfiguredBuilder* (*CreateBuilderFunc)();

static ConfiguredBuilder* createDatabase()
{
	static module::ConfiguredBuilderDescription< db::OracleTestConstructor,
		db::OracleTestConfig > mod( "test Oracle database", "database", "OracleTest", db::ORACLE_DB_CLASS_NAME);
	return &mod;
}

enum { NofObjects = 1 };
static CreateBuilderFunc containers[ NofObjects] =
{
	createDatabase
};

ModuleEntryPoint entryPoint( 0, "Oracle test database", NofObjects, containers, 0, 0);

}} // namespace _Wolframe::module

