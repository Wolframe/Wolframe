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
//
// common part which has to be known to Wolframe and the loadable module
//

#include "moduleInterface.hpp"
#include "database/DBprovider.hpp"

namespace _Wolframe {
namespace module {
namespace test {

class TestModuleConfig :  public config::ObjectConfiguration
{
	friend class TestModuleContainer;

public:
	TestModuleConfig( const char* cfgName, const char* logParent, const char* logName );
	
	virtual ~TestModuleConfig( ) {}

	virtual const char* objectName() const		{ return "TestUnitBase"; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string m_a_param;
};

// must be an abstract base class for usage in the code loading
// and using objects of the derived class!
class TestUnitBase
{
public:
	// may not be private and must be virtual, can be empty as
	// we define an interface here only
	virtual ~TestUnitBase( ) { };

	// must be virtual, otherwise moduleTest tries to link a hello
	// function in which can't exist there!
	virtual const std::string hello( ) = 0;
};

class TestModuleContainer : public ObjectContainer< TestUnitBase >
{
public:
	TestModuleContainer( const TestModuleConfig& conf );
	
	~TestModuleContainer()			{}

	virtual const char* objectName() const	{ return "TestUnitBase"; }
	virtual TestUnitBase* object() const	{ return m_test; }

private:
	TestUnitBase *m_test;	
};

}}} // namespace _Wolframe::module::test
