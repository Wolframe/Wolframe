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
// Wolframe SQLite client view implementation
//

#include "logger/logger-v1.hpp"
#include "SQLiteTest.hpp"
#include "utils/fileUtils.hpp"
#include "sqlite3.h"
#include <boost/system/error_code.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

#ifndef _WIN32
#define OPERATING_SYSTEM "unix"
#else
#define OPERATING_SYSTEM 0
#endif

static void removeFileIfExists( const std::string& filename)
{
	try
	{
		boost::filesystem::remove( filename);
	}
	catch (const boost::filesystem::filesystem_error& err)
	{
		throw std::runtime_error( std::string( "failed to remove file '") + filename + "': " + err.what());
	}
	catch (std::exception& err)
	{
		throw std::runtime_error( std::string("error removing file: '") + err.what() + "'");
	}
}

void SQLiteTestConstructor::createTestDatabase( const std::string& filename, const std::string& inputfile)
{
	removeFileIfExists( filename);
	static bool sqlite3_config_called = false;
	if (!sqlite3_config_called)
	{
		if (sqlite3_config( SQLITE_CONFIG_SINGLETHREAD) != SQLITE_OK)
		{
			throw std::logic_error( "unable to configure SQLite test database" );
		}
		sqlite3_config_called = true;
	}
	sqlite3* handle = 0;
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;
	int res = sqlite3_open_v2( filename.c_str(), &handle, flags, OPERATING_SYSTEM);
	boost::shared_ptr<sqlite3> handle_disposer( handle, sqlite3_close);
	if (res != SQLITE_OK)
	{
		throw std::runtime_error( "unable to create SQLite test database" );
	}

	char* err = 0;
	std::string dbsource = utils::readSourceFileContent( inputfile);
	res = sqlite3_exec( handle, dbsource.c_str(), NULL, NULL, &err);
	if (res != SQLITE_OK)
	{
		std::string msg( err?err:"unknown error");
		sqlite3_free( err);
		throw std::runtime_error( std::string( "error creating SQLite test database: '") + msg + "'");
	}
}

config::ConfigurationTree SQLiteTestConfig::extractMyNodes( const config::ConfigurationTree& pt)
{
	boost::property_tree::ptree rt;
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();

	for (; pi != pe; ++pi)
	{
		if (boost::algorithm::iequals( pi->first, "inputfile" ))
		{
			m_input_filename = pi->second.data();
		}
		else if (boost::algorithm::iequals( pi->first, "dumpfile" ))
		{
			m_dump_filename = pi->second.data();
		}
		else
		{
			rt.add_child( pi->first, pi->second);
		}
	}
	return rt;
}

void SQLiteTestConfig::setMyCanonicalPathes( const std::string& referencePath)
{
	if (!m_input_filename.empty())
	{
		m_input_filename = utils::getCanonicalPath( m_input_filename, referencePath);
	}
	if (!m_dump_filename.empty())
	{
		m_dump_filename = utils::getCanonicalPath( m_dump_filename, referencePath);
	}
}

static int dump_callback( void* fh_, int colno, char** col, char**)
{
	char buf[ 4096];
	FILE* fh = (FILE*)fh_;
	for (int ii=0; ii < colno; ii++)
	{
		const char* elem = sqlite3_snprintf( sizeof(buf), buf, "%Q", col[ii]);
		fprintf( fh, ii?", %s":"%s", elem);
	}
	fprintf( fh, "\n");
	return 0;
}

static void dump_table( FILE* fh, sqlite3 *db, const char* tablename)
{
	std::string sql( "SELECT * FROM ");
	sql.append( tablename);
	sql.append( ";");
	char *errs = NULL;

	fprintf( fh, "%s:\n", tablename);
	if (sqlite3_exec( db, sql.c_str(), dump_callback, (void*)fh, &errs) != SQLITE_OK)
	{
		std::string msg( errs?errs:"unknown error");
		sqlite3_free( errs);
		throw std::runtime_error( std::string("failed to dump table ") + tablename + ": " + msg);
	}
	fprintf( fh, "\n");
}

static std::vector<std::string> getTableNames( sqlite3* handle)
{
	static const char* tableselect = "SELECT name FROM sqlite_master WHERE type='table';";
	std::vector<std::string> rt;
	sqlite3_stmt* stm = 0;

	int rc = sqlite3_prepare_v2( handle, tableselect, -1, &stm, 0);
	if (rc == SQLITE_OK)
	{
		boost::shared_ptr<sqlite3_stmt> stm_disposer( stm, sqlite3_finalize);
		while ((rc = sqlite3_step( stm)) == SQLITE_ROW)
		{
			const char* tablename = (const char*)sqlite3_column_text( stm, 0);
			rt.push_back( tablename);
		}
	}
	return rt;
}

void SQLiteTestConfig::dump_database()
{
	FILE *fh = fopen( m_dump_filename.c_str(), "w");
	if (fh == NULL) throw std::runtime_error( std::string("failed to open file for database dump (") + boost::lexical_cast<std::string>(errno) + "), file '" + m_dump_filename + "'");
	boost::shared_ptr<FILE> file_closer( fh, fclose);

	sqlite3* handle;
	int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX;
	int res = sqlite3_open_v2( filename().c_str(), &handle, flags, OPERATING_SYSTEM);
	boost::shared_ptr<sqlite3> handle_disposer( handle, sqlite3_close);

	if (res != SQLITE_OK)
	{
		throw std::runtime_error( "unable to open SQLite test database for dump");
	}
	std::vector<std::string> tablenames = getTableNames( handle);
	std::vector<std::string>::const_iterator ti = tablenames.begin(), te = tablenames.end();
	for (; ti != te; ++ti)
	{
		dump_table( fh, handle, ti->c_str());
	}
}


