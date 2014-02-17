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
// job schedule in database
//

#include "processor/jobSchedule.hpp"
#include "module/constructor.hpp"
#include "database/DBprovider.hpp"

#ifndef _SCHEDULE_DATABASE_HPP_INCLUDED
#define _SCHEDULE_DATABASE_HPP_INCLUDED

namespace _Wolframe {
namespace processor {

static const char* JOB_SCHEDULE_CLASS_NAME = "JobSchedule";

class JobScheduleDBconfig : public config::NamedConfiguration
{
	friend class JobScheduleDBconstructor;
public:
	JobScheduleDBconfig( const char* cfgName, const char* logParent, const char* logName )
		: config::NamedConfiguration( cfgName, logParent, logName )
	{ }

	const char* className() const		{ return JOB_SCHEDULE_CLASS_NAME; }

	/// methods
	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	std::string		m_dbConfig;
};


class JobScheduleDB : public JobSchedule
{
public:
	JobScheduleDB( const std::string& dbLabel );
	~JobScheduleDB();
	const char* className() const		{ return JOB_SCHEDULE_CLASS_NAME; }

	bool resolveDB( const db::DatabaseProvider& db );

private:
	std::string		m_dbLabel;
	const db::Database*	m_db;
};

class JobScheduleDBconstructor : public ConfiguredObjectConstructor< JobSchedule >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return JOB_SCHEDULE_OBJECT; }
	const char* objectClassName() const	{ return JOB_SCHEDULE_CLASS_NAME; }
	JobScheduleDB* object( const config::NamedConfiguration& conf );
};

}} // namespace _Wolframe::processor

#endif // _SCHEDULE_DATABASE_HPP_INCLUDED
