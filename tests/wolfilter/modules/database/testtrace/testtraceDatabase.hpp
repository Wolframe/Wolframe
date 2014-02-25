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
///\file modules/database/testtrace/testtraceDatabase.hpp
///\brief Interface of fake database implementation for testing database processing engines
#ifndef _TESTTRACE_FAKE_DATABASE_HPP_INCLUDED
#define _TESTTRACE_FAKE_DATABASE_HPP_INCLUDED
#include "database/database.hpp"
#include "config/configurationBase.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "module/constructor.hpp"
#include <list>
#include <string>

namespace _Wolframe {
namespace db {

#define TESTTRACE_DATABASE_CLASSNAME "TesttraceDatabase"


///\class TesttraceConfig
///\brief Fake database configuration
class TesttraceDatabaseConfig
	:public config::NamedConfiguration
{
public:
	TesttraceDatabaseConfig( const char* name_, const char* logParent_, const char* logName_)
		:config::NamedConfiguration( name_, logParent_, logName_){}

	~TesttraceDatabaseConfig(){}

	virtual const char* className() const
	{
		return TESTTRACE_DATABASE_CLASSNAME;
	}

	virtual bool parse( const config::ConfigurationTree& pt, const std::string& node, const module::ModulesDirectory* modules);
	virtual bool check() const;
	void print( std::ostream& os, size_t indent) const;
	virtual void setCanonicalPathes( const std::string& referencePath);

	const std::string& resultfilename() const	{return m_data.resultfilename;}
	const std::string& outfilename() const		{return m_data.outfilename;}
	const std::string& id() const			{return m_data.id;}

private:
	struct Data
	{
		std::string resultfilename;
		std::string outfilename;
		std::string id;
		static const serialize::StructDescriptionBase* getStructDescription();
	};
	Data m_data;
};


///\class TesttraceDatabase
///\brief Testtrace fake database instance
class TesttraceDatabase
	:public Database
{
public:
	TesttraceDatabase( const std::string& id_, const std::string& resultfilename_, const std::string& outfilename_);
	TesttraceDatabase( const TesttraceDatabase& o)
		:Database()
		,m_id(o.m_id)
		,m_outfilename(o.m_outfilename)
		,m_result(o.m_result){}

	virtual ~TesttraceDatabase(){}

	virtual const std::string& ID() const
	{
		return m_id;
	}

	virtual const char* className() const
	{
		return TESTTRACE_DATABASE_CLASSNAME;
	}

	virtual Transaction* transaction( const std::string& /*name*/ );

	virtual void addProgram( const std::string& source);
	virtual void loadProgram( const std::string& filename);
	virtual void loadAllPrograms(){}

	const std::string& outfilename() const
	{
		return m_outfilename;
	}

private:
	std::string m_id;
	std::string m_outfilename;
	std::vector<std::string> m_result;
};

///\class TesttraceDatabaseUnit
///\brief Testtrace fake database constructor
class TesttraceDatabaseUnit
	:public DatabaseUnit
{
public:
	TesttraceDatabaseUnit( const std::string& id_, const std::string& resultfilename_, const std::string& outfilename_)
		:m_id(id_)
		,m_db(id_,resultfilename_,outfilename_)
		{}
	TesttraceDatabaseUnit( const TesttraceDatabaseUnit& o)
		:DatabaseUnit()
		,m_id(o.m_id)
		,m_db(o.m_db)
		{}

	virtual ~TesttraceDatabaseUnit(){}

	virtual const std::string& ID() const
	{
		return m_id;
	}

	virtual const char* className() const
	{
		return TESTTRACE_DATABASE_CLASSNAME;
	}

	virtual Database* database()
	{
		return &m_db;
	}

	virtual void addProgram( const std::string& source)
	{
		m_db.addProgram( source);
	}

	virtual void loadProgram( const std::string& filename)
	{
		m_db.loadProgram( filename);
	}

	virtual void loadAllPrograms(){}

private:
	std::string m_id;
	TesttraceDatabase m_db;
};


///\class TesttraceDatabaseConstructor
///\brief Testtrace fake database constructor
class TesttraceDatabaseConstructor : public ConfiguredObjectConstructor<db::DatabaseUnit>
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return DATABASE_OBJECT;
	}

	virtual const char* objectClassName() const
	{
		return TESTTRACE_DATABASE_CLASSNAME;
	}

	virtual TesttraceDatabaseUnit* object( const config::NamedConfiguration& conf_)
	{
		const TesttraceDatabaseConfig& conf = dynamic_cast<const TesttraceDatabaseConfig&>( conf_);
		return new TesttraceDatabaseUnit( conf.id(), conf.resultfilename(), conf.outfilename());
	}
};

}} // _Wolframe::db

#endif

