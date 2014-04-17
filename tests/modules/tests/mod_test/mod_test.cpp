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

#include "module/moduleInterface.hpp"
#include "logger-v1.hpp"
#include "mod_test.hpp"

namespace _Wolframe {
namespace module {
namespace test {

TestModuleConfig::TestModuleConfig( const char* cfgName, const char* logParent, const char* logName )
	: config::NamedConfiguration( cfgName, logParent, logName )
{
	LOG_DEBUG << "Module: test module config created";
}

TestModuleConfig::~TestModuleConfig()
{
	LOG_DEBUG << "Module: test module config destroyed";
}

bool TestModuleConfig::parse( const config::ConfigurationNode& /*pt*/, const std::string& /*node*/,
    const module::ModulesDirectory* /*modules*/ )
{
	return true;
}

bool TestModuleConfig::check( ) const
{
	return true;
}

void TestModuleConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": no config " << std::endl;
}

void TestModuleConfig::setCanonicalPathes( const std::string& /*refPath*/ )
{
}

TestUnit* TestModuleConstructor::object( const config::NamedConfiguration& /* conf */ )
{
	TestUnit* m_test = new TestUnitImpl( /* conf */ );
	LOG_DEBUG << "Module: test module object created";
	return m_test;
}

TestUnitImpl::TestUnitImpl( )
{
	LOG_DEBUG << "Module: testUnit object created";
}

TestUnitImpl::~TestUnitImpl( )
{
	LOG_DEBUG << "Module: testUnit object destroyed";
}

const std::string TestUnitImpl::hello( )
{
	LOG_ALERT << "MODULE: hello() called";
	return "hello";
}

bool TestUnitImpl::resolveDB( const db::DatabaseProvider& /* db */ )
{
	return true;
}

static BaseBuilder* createModule( void )
{
	static module::ConfiguredBuilderDescription< test::TestModuleConstructor,
		test::TestModuleConfig > mod( "Test Module", "Test", "test", "TestObject" );
	return &mod;
}


static BaseBuilder* (*containers[])() = {
	createModule, NULL
};

ModuleEntryPoint entryPoint( 0, "Test Module", containers );

}}} // namespace _Wolframe::module:test
