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
///\file modules/database/testtrace/testtraceDatabase.cpp
///\brief Fake database implementation for testing database processing engines
#include "logger-v1.hpp"
#include "testtraceDatabase.hpp"
#include "testtraceTransaction.hpp"
#include "config/programBase.hpp"
#include "serialize/struct/filtermapDescription.hpp"
#include "config/structSerialize.hpp"
#include "utils/fileUtils.hpp"
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
			( "identifier",	&This::id)
			( "file",	&This::resultfilename)
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
	if (!m_data.resultfilename.empty() && !utils::fileExists( m_data.resultfilename))
	{
		LOG_ERROR << "Configured result file '" << m_data.resultfilename << "' does not exist";
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
	if (!m_data.resultfilename.empty())
	{
		m_data.resultfilename = utils::getCanonicalPath( m_data.resultfilename, referencePath);
	}
	if (!m_data.outfilename.empty())
	{
		m_data.outfilename = utils::getCanonicalPath( m_data.outfilename, referencePath);
	}
}


TesttraceDatabase::TesttraceDatabase( const std::string& id_, const std::string& resultfilename_, const std::string& outfilename_, unsigned short, bool)
	:m_id(id_)
	,m_outfilename(outfilename_)
{
	if (!resultfilename_.empty())
	{
		m_result = utils::readSourceFileLines( resultfilename_);
	}
}

Transaction* TesttraceDatabase::transaction(const std::string& /*name*/ )
{
	return new TesttraceTransaction( this, m_result);
}

void TesttraceDatabase::addProgram( const std::string& source)
{
	config::PositionalErrorMessageBase ERROR(source);
	config::PositionalErrorMessageBase::Message MSG;
	static const utils::CharTable g_optab( ";:-,.=)(<>[]/&%*|+-#?!$");
	std::string::const_iterator si = source.begin(), se = source.end();
	char ch;
	std::string tok;
	const char* commentopr = "--";

	while ((ch = utils::parseNextToken( tok, si, se, g_optab)) != 0)
	{
		if (ch == commentopr[0])
		{
			std::size_t ci = 1;
			while (!commentopr[ci] && commentopr[ci] == *si)
			{
				ci++;
				si++;
			}
			if (!commentopr[ci])
			{
				// skip to end of line
				while (si != se && *si != '\n') ++si;
			}
		}
		else if (g_optab[ch])
		{
			throw ERROR( si, MSG << "unexpected token '" << ch << "'");
		}
		else
		{
			throw ERROR( si, MSG << "unexpected token in DB source '" << tok << "'");
		}
	}
}

void TesttraceDatabase::loadProgram( const std::string& filename)
{
	std::string dbsource = utils::readSourceFileContent( filename);
	addProgram( dbsource);
}

