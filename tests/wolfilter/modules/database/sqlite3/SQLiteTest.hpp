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
// Wolframe SQLite test client
//
#ifndef _SQLITE_TEST_HPP_INCLUDED
#define _SQLITE_TEST_HPP_INCLUDED
#include "SQLite.hpp"
#include <boost/property_tree/ptree.hpp>
#include "config/ConfigurationTree.hpp"

namespace _Wolframe {
namespace db {

///\class SQLiteTestConfig
///\brief SQLite test database configuration
class SQLiteTestConfig : public SQLiteConfig
{
public:
	SQLiteTestConfig( const char* name, const char* logParent, const char* logName )
		:SQLiteConfig( name, logParent, logName){}

	virtual ~SQLiteTestConfig()
	{
		dump_database();
	}

	virtual bool parse( const config::ConfigurationTree& pt, const std::string& node,
				const module::ModulesDirectory* modules )
	{
		return SQLiteConfig::parse( extractMyNodes( pt), node, modules);
	}

	virtual void setCanonicalPathes( const std::string& referencePath)
	{
		SQLiteConfig::setCanonicalPathes( referencePath);
		setMyCanonicalPathes( referencePath);
	}

	const std::string& input_filename() const	{return m_input_filename;}
	const std::string& dump_filename() const	{return m_dump_filename;}

private:
	config::ConfigurationTree extractMyNodes( const config::ConfigurationTree& pt);
	void setMyCanonicalPathes( const std::string& referencePath);
	void dump_database();

	std::string m_input_filename;
	std::string m_dump_filename;
};

class SQLiteTestConstructor : public SQLiteConstructor
{
public:
	SQLiteTestConstructor(){}
	virtual ~SQLiteTestConstructor(){}

	virtual SQLiteDBunit* object( const config::NamedConfiguration& conf)
	{
		const SQLiteTestConfig& cfg = dynamic_cast< const SQLiteTestConfig&>( conf);
		createTestDatabase( cfg.filename(), cfg.input_filename());
		return SQLiteConstructor::object( conf);
	}

private:
	static void createTestDatabase( const std::string& filename_, const std::string& inputfile_);
};

}} // _Wolframe::db

#endif
