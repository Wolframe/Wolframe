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
//
// a test module
//

#include "moduleInterface.hpp"
#include "logger/logger-v1.hpp"
#include "mod_test.hpp"

_Wolframe::log::LogBackend*	logBackendPtr;

namespace _Wolframe {
namespace module {
namespace test {

TestModuleConfig::TestModuleConfig( const char* cfgName, const char* logParent, const char* logName )
	: config::NamedConfiguration( cfgName, logParent, logName )
{
	MOD_LOG_DEBUG << "Test module config created";
}

TestModuleConfig::~TestModuleConfig()
{
	MOD_LOG_DEBUG << "Test module config destroyed";
}

bool TestModuleConfig::parse( const config::ConfigurationTree& /*pt*/, const std::string& /*node*/,
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
	MOD_LOG_DEBUG << "Test module object created";
	return m_test;
}

TestUnitImpl::TestUnitImpl( )
{
	MOD_LOG_DEBUG << "TestUnit object created";
}

TestUnitImpl::~TestUnitImpl( )
{
	MOD_LOG_DEBUG << "TestUnit object destroyed";
}

const std::string TestUnitImpl::hello( )
{
	return "hello";
}

bool TestUnitImpl::resolveDB( const db::DatabaseProvider& /* db */ )
{
	return true;
}

static ConfiguredBuilder* createModule( void )
{
	static module::ConfiguredBuilderDescription< test::TestModuleConstructor,
		test::TestModuleConfig > mod( "Test Module", "Test", "test", "TestObject" );
	return &mod;
}

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend* >( logger );
}


static const unsigned short nrContainers = 1;
static ConfiguredBuilder* (*containers[ nrContainers ])() = {
	createModule
};

ModuleEntryPoint entryPoint( 0, "Test Module", setModuleLogger,
			     nrContainers, containers,
			     0, NULL );

}}} // namespace _Wolframe::module:test
