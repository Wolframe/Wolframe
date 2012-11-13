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
//
// Wolframe PostgreSQL test client implementation
//
#include "logger-v1.hpp"
#include "PostgreSQLTest.hpp"
#include "utils/miscUtils.hpp"
#include <boost/system/error_code.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

void PostgreSQLTestConstructor::createTestDatabase( const PostgreSQLTestConfig& )
{
}

config::ConfigurationTree PostgreSQLTestConfig::extractMyNodes( const config::ConfigurationTree& pt )
{
	boost::property_tree::ptree rt;
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();

	for ( ; pi != pe; ++pi )	{
		if (boost::algorithm::iequals( pi->first, "inputfile" ))
			m_input_filename = pi->second.data();
		else if (boost::algorithm::iequals( pi->first, "dumpfile" ))
			m_dump_filename = pi->second.data();
		else
			rt.add_child( pi->first, pi->second );
	}
	return rt;
}

void PostgreSQLTestConfig::setMyCanonicalPathes( const std::string& referencePath )
{
	if ( !m_input_filename.empty() )
		m_input_filename = utils::getCanonicalPath( m_input_filename, referencePath );
	if (!m_dump_filename.empty())
		m_dump_filename = utils::getCanonicalPath( m_dump_filename, referencePath );
}

void PostgreSQLTestConfig::dump_database()
{
}


