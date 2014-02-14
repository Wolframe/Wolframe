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
// Wolframe Postgres test client
//
#ifndef _POSTGRES_TEST_HPP_INCLUDED
#define _POSTGRES_TEST_HPP_INCLUDED
#include "PostgreSQL.hpp"
#include <boost/property_tree/ptree.hpp>
#include "config/ConfigurationTree.hpp"

namespace _Wolframe {
namespace db {

///\class PostgreSQLTestConfig
///\brief PostgreSQL test database configuration
class PostgreSQLTestConfig : public PostgreSQLconfig
{
public:
	PostgreSQLTestConfig( const char* name, const char* logParent, const char* logName )
		: PostgreSQLconfig( name, logParent, logName )	{}

	virtual ~PostgreSQLTestConfig()
	{
		dump_database();
	}

	virtual bool parse( const config::ConfigurationTree& pt, const std::string& node,
			    const module::ModulesDirectory* modules )
	{
		return PostgreSQLconfig::parse( extractMyNodes( pt), node, modules );
	}

	virtual void setCanonicalPathes( const std::string& referencePath )
	{
		PostgreSQLconfig::setCanonicalPathes( referencePath );
		setMyCanonicalPathes( referencePath );
	}

	const std::string& input_filename() const	{ return m_input_filename; }
	const std::string& dump_filename() const	{ return m_dump_filename; }

private:
	config::ConfigurationTree extractMyNodes( const config::ConfigurationTree& pt );
	void setMyCanonicalPathes( const std::string& referencePath );
	void dump_database();

	std::string m_input_filename;
	std::string m_dump_filename;
};

class PostgreSQLTestConstructor : public PostgreSQLconstructor
{
public:
	PostgreSQLTestConstructor()		{}
	virtual ~PostgreSQLTestConstructor()	{}

	virtual PostgreSQLdbUnit* object( const config::NamedConfiguration& conf )
	{
		const PostgreSQLTestConfig& cfg = dynamic_cast< const PostgreSQLTestConfig& >( conf );
		createTestDatabase( cfg );
		return PostgreSQLconstructor::object( conf );
	}

private:
	static void createTestDatabase( const PostgreSQLTestConfig& cfg );
};

}} // _Wolframe::db

#endif
