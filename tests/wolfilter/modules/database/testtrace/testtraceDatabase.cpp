/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\file modules/database/testtrace/testtraceDatabase.cpp
///\brief Fake database implementation for testing database processing engines
#include "logger-v1.hpp"
#include "testtraceDatabase.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "config/structSerialize.hpp"
#include "utils/miscUtils.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

const serialize::StructDescriptionBase* TesttraceDatabaseConfig::Data::getStructDescription()
{
	typedef TesttraceDatabaseConfig::Data This;
	struct ThisDescription :public serialize::StructDescription<This>
	{
		ThisDescription()
		{
			(*this)
			( "id",		&This::id)
			( "file",	&This::filename)
			( "outfile",	&This::outfilename)
			;
		}
	};
	static const ThisDescription rt;
	return &rt;
}

bool TesttraceDatabaseConfig::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	try
	{
		config::parseConfigStructure( m_data, (const boost::property_tree::ptree&)pt);
		return true;
	}
	catch (std::exception& e)
	{
		LOG_ERROR << e.what();
		return false;
	}
}

bool TesttraceDatabaseConfig::check() const
{
	if (!m_data.filename.empty() && !utils::fileExists( m_data.filename))
	{
		LOG_ERROR << "Configured file '" << m_data.filename << "' does not exist";
		return false;
	}
	if (m_data.outfilename.empty())
	{
		LOG_ERROR << "Output file of test trace DB is not configured";
		return false;
	}
	return true;
}

void TesttraceDatabaseConfig::print( std::ostream& os, size_t indent) const
{
	std::string indentstr( indent+1, '\t');
	indentstr[0] = '\n';
	std::string rt( config::structureToString( m_data));
	boost::replace_all( rt, "\n", indentstr);
	os << rt;
}

void TesttraceDatabaseConfig::setCanonicalPathes( const std::string& referencePath)
{
	if (!m_data.filename.empty())
	{
		m_data.filename = utils::getCanonicalPath( m_data.filename, referencePath);
	}
	if (!m_data.outfilename.empty())
	{
		m_data.outfilename = utils::getCanonicalPath( m_data.outfilename, referencePath);
	}
}


TesttraceDatabase::TesttraceDatabase( const std::string& id_, const std::string& filename_, const std::string& outfilename_, unsigned short, bool)
	:m_id(id_)
	,m_outfilename(outfilename_)
{
	if (!filename_.empty())
	{
		m_result = utils::readSourceFileLines( filename_);
	}
}



