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
///\file testWolfilter.cpp
///\brief Test program for wolfilter like stdin/stdout mapping
#include "database/processor.hpp"
#include "database/preparedStatement.hpp"
#include "langbind/appGlobalContext.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "testDescription.hpp"
#include "filter/token_filter.hpp"
#include "filter/textwolf_filter.hpp"
#include "filter/typingfilter.hpp"
#include "filter/tostringfilter.hpp"
#include "utils/miscUtils.hpp"
#include "processor/procProvider.hpp"
#include "moduleInterface.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};
static boost::filesystem::path g_testdir;

using namespace _Wolframe;
using namespace _Wolframe::db;

static std::size_t getResultIndex( const std::string& testname)
{
	static std::map<std::string, std::size_t> g_resultitrmap;
	static boost::mutex g_resultitrmap_mutex;

	boost::interprocess::scoped_lock<boost::mutex> lock(g_resultitrmap_mutex);
	return ++g_resultitrmap[ testname];
}

static std::vector<std::vector<std::string> > readResultFile( boost::filesystem::path& path)
{
	char buf;
	std::fstream ff;
	std::string line;
	std::vector< std::vector<std::string> > rt;
	ff.open( path.string().c_str(), std::ios::in);
	while (ff.read( &buf, sizeof(buf)))
	{
		if (buf == '\n')
		{
			std::string tok;
			std::string::const_iterator il = line.begin(), el = line.end();
			std::vector<std::string> row;

			while (utils::parseNextToken( tok, il, el))
			{
				row.push_back( tok);
			}
			rt.push_back( row);
			line.clear();
		}
		else
		{
			line.push_back( buf);
		}
	}
	if ((ff.rdstate() & std::ifstream::eofbit) == 0)
	{
		std::ostringstream msg;
		msg << "failed to result from file: '" << path << "'";
		throw std::runtime_error( msg.str());
	}
	ff.close();
	return rt;
}


struct Result
{
public:
	Result(){}
	void load( std::size_t idx)
	{
		boost::filesystem::path path( g_testdir / "temp" / (boost::lexical_cast<std::string>(idx) + ".result"));
		if (utils::fileExists( path.string()))
		{
			m_data = readResultFile( path);
			m_itr = m_data.begin();
			if (m_itr == m_data.end())
			{
				throw std::runtime_error( "empty result");
			}
			m_cols = *m_itr++;
		}
		else
		{
			m_cols.clear();
			m_data.clear();
			m_itr = m_data.begin();
		}
	}

	static void removeAll()
	{
		static boost::mutex this_mutex;
		boost::interprocess::scoped_lock<boost::mutex> lock(this_mutex);

		for (std::size_t idx=1; ;idx++)
		{
			boost::filesystem::path path( g_testdir / "temp" / (boost::lexical_cast<std::string>(idx) + ".result"));
			if (utils::fileExists( path.string()))
			{
				boost::filesystem::remove( path);
			}
			else
			{
				break;
			}
		}
	}

	bool next()					{if (m_itr != m_data.end()) ++m_itr; return (m_itr != m_data.end());}
	unsigned int nofColumns()			{return m_cols.size();}
	const char* columnName( std::size_t idx)	{return (idx == 0 || idx > m_cols.size())?0:m_cols[idx-1].c_str();}
	const char* get( std::size_t idx)		{return (m_itr == m_data.end() || idx == 0 || idx > m_itr->size())?0:(*m_itr)[idx-1].c_str();}

private:
	typedef std::vector<std::string> Row;
	std::vector<Row> m_data;
	std::vector<Row>::const_iterator m_itr;
	std::vector<std::string> m_cols;
};


class DatabaseCommandLog
	:public PreparedStatementHandler
{
public:
	DatabaseCommandLog( const std::string& name)
		:m_testname(name){}

	virtual bool begin()
	{
		m_out << "begin();" << std::endl;
		return true;
	}

	virtual bool commit()
	{
		m_out << "commit();" << std::endl;
		return true;
	}

	virtual bool rollback()
	{
		m_out << "rollback();" << std::endl;
		return true;
	}

	virtual bool start( const std::string& stmname)
	{
		m_out << "start( '" << stmname << "' );" << std::endl;
		return true;
	}

	virtual bool bind( std::size_t idx, const char* value)
	{
		if (value)
		{
			m_out << "bind( " << idx << ", '" << value << "' );" << std::endl;
		}
		else
		{
			m_out << "bind( " << idx << ", NULL );" << std::endl;
		}
		return true;
	}

	virtual bool execute()
	{
		m_res.load( getResultIndex( m_testname));
		m_out << "execute();" << std::endl;
		return true;
	}

	virtual std::size_t nofColumns()
	{
		m_out << "nofColumns(); returns " << m_res.nofColumns() << std::endl;
		return m_res.nofColumns();
	}

	virtual const char* columnName( std::size_t idx)
	{
		m_out << "columnName( " << idx << " ); returns " << m_res.columnName( idx) << std::endl;
		return m_res.columnName( idx);
	}

	virtual const char* getLastError()
	{
		m_out << "getLastError(); returns 0" << std::endl;
		return 0;
	}

	virtual const char* get( std::size_t idx)
	{
		m_out << "get( " << idx << " ); returns " << m_res.get( idx) << std::endl;
		return m_res.get( idx);
	}

	virtual bool next()
	{
		bool rt = m_res.next();
		m_out << "next(); returns " << rt << std::endl;
		return rt;
	}

	std::string str()
	{
		return m_out.str();
	}

	void reset()
	{
		m_out.str("");
	}
private:
	std::string m_testname;
	std::ostringstream m_out;
	Result m_res;
};

static std::map<std::string, PreparedStatementHandlerR> g_testoutput;
static boost::mutex g_testoutput_mutex;

static std::string getTestOutput( const std::string& testname)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(g_testoutput_mutex);
	PreparedStatementHandlerR stm = g_testoutput[ testname];
	DatabaseCommandLog* log = dynamic_cast<DatabaseCommandLog*>( stm.get());
	return log->str();
}

static PreparedStatementHandlerR createPreparedStatementHandlerFunc( const std::string& testname)
{
	boost::interprocess::scoped_lock<boost::mutex> lock(g_testoutput_mutex);
	PreparedStatementHandlerR rt( new DatabaseCommandLog( testname));
	g_testoutput[ testname] = rt;
	return rt;
}

static proc::ProcProviderConfig g_processorProviderConfig;
static proc::ProcessorProvider* g_processorProvider = 0;
static module::ModulesDirectory g_modulesDirectory;

///\brief Loads the modules, scripts, etc. defined hardcoded and in the command line into the global context
static void loadGlobalContext( const std::string& testname)
{
	if (g_processorProvider) delete g_processorProvider;
	g_processorProvider = new proc::ProcessorProvider( &g_processorProviderConfig, &g_modulesDirectory);
	langbind::GlobalContext* gct = new langbind::GlobalContext( g_processorProvider);
	langbind::defineGlobalContext( langbind::GlobalContextR( gct));

	std::string cmdname( "printcmd");
	gct->definePreparedStatementHandler( cmdname, testname, &createPreparedStatementHandlerFunc);
}

class DatabaseProcessorVMTest : public ::testing::Test
{
protected:
	DatabaseProcessorVMTest() {}
	virtual ~DatabaseProcessorVMTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string selectedTestName;

void pushTestInput( const langbind::TransactionFunction::InputR& input, std::string tdinput)
{
	langbind::Filter tokenfilter = langbind::createTokenFilter( "token", "UTF-8");
	langbind::Filter xmlfilter = langbind::createTextwolfXmlFilter( "xml:textwolf", "");
	langbind::TypingInputFilter inp( xmlfilter.inputfilter());

	xmlfilter.inputfilter()->setValue( "empty", "false");
	xmlfilter.inputfilter()->putInput( tdinput.c_str(), tdinput.size(), true);

	langbind::TypedFilterBase::ElementType type;
	langbind::TypedFilterBase::Element element;
	int taglevel = 0;
	while (inp.getNext( type, element))
	{
		if (type == langbind::FilterBase::OpenTag)
		{
			taglevel++;
		}
		else if (type == langbind::FilterBase::CloseTag)
		{
			taglevel--;
		}
		if (taglevel >= 0)
		{
			if (!input->print( type, element)) throw std::runtime_error( "token filter buffer overflow");
		}
		else
		{
			const void* ptr;
			std::size_t ii,size;
			bool end;
			xmlfilter.inputfilter()->getRest( ptr, size, end);
			for (ii=0; ii<size; ++ii)
			{
				if (((const char*)ptr)[ii] < 0 || ((const char*)ptr)[ii] > 32)
				{
					throw std::runtime_error( "unconsumed input left");
				}
			}
			break;
		}
	}
}


TEST_F( DatabaseProcessorVMTest, tests)
{
	std::vector<std::string> tests;
	std::size_t testno;

	// [1] Selecting tests to execute:
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / "databaseProcessorVM" / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}

	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			std::string testname = boost::filesystem::basename(*ditr);
			if (selectedTestName.size())
			{
				if (std::strstr( ditr->path().string().c_str(), selectedTestName.c_str()))
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
					tests.push_back( ditr->path().string());
				}
			}
			else
			{
				tests.push_back( ditr->path().string());
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << *ditr << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());

	// [2] Execute tests:
	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (testno=1; itr != end; ++itr,++testno)
	{
		Result::removeAll();
		std::string testname = boost::filesystem::basename(*itr);

		wtest::TestDescription td( *itr, g_gtest_ARGV[0]);
		if (td.requires.size())
		{
			// [2.2] Skip tests when disabled
			std::cerr << "skipping test '" << testname << "' ( is " << td.requires << ")" << std::endl;
			continue;
		}

		std::cerr << "processing test '" << testname << "'" << std::endl;
		loadGlobalContext( testname);

		TransactionFunction program( "printcmd", td.config);
		langbind::TransactionFunction::InputR input = program.getInput();
		pushTestInput( input, td.input);
		langbind::TransactionFunction::ResultR result = program.execute( input.get());
		langbind::ToStringFilter* resultbuf = new langbind::ToStringFilter( "  ");
		langbind::TypedOutputFilterR resultbufref( resultbuf);
		langbind::RedirectFilterClosure redirect( result, resultbufref);
		if (!redirect.call()) throw std::runtime_error( "to string conversion of database output failed");

		std::string output = getTestOutput( testname);
		std::string resultstr = resultbuf->content();
		if (resultstr.size())
		{
			output.append( "\n");
			output.append( resultstr);
		}
		if (td.expected != output)
		{
			static boost::mutex mutex;
			boost::interprocess::scoped_lock<boost::mutex> lock(mutex);

			// [2.6] Dump test contents to files in case of error
			boost::filesystem::path OUTPUT( g_testdir / "temp" / "OUTPUT");
			std::fstream oo( OUTPUT.string().c_str(), std::ios::out | std::ios::binary);
			oo.write( output.c_str(), output.size());
			if (oo.bad()) std::cerr << "error writing file '" << OUTPUT.string() << "'" << std::endl;
			oo.close();

			boost::filesystem::path EXPECT( g_testdir / "temp" / "EXPECT");
			std::fstream ee( EXPECT.string().c_str(), std::ios::out | std::ios::binary);
			ee.write( td.expected.c_str(), td.expected.size());
			if (ee.bad()) std::cerr << "error writing file '" << EXPECT.string() << "'" << std::endl;
			ee.close();

			boost::filesystem::path INPUT( g_testdir / "temp" / "INPUT");
			std::fstream ss( INPUT.string().c_str(), std::ios::out | std::ios::binary);
			ss.write( td.input.c_str(), td.input.size());
			if (ss.bad()) std::cerr << "error writing file '" << INPUT.string() << "'" << std::endl;
			ss.close();

			std::cerr << "test output does not match for '" << *itr << "'" << std::endl;
			std::cerr << "INPUT  written to file '"  << INPUT.string() << "'" << std::endl;
			std::cerr << "OUTPUT written to file '" << OUTPUT.string() << "'" << std::endl;
			std::cerr << "EXPECT written to file '" << EXPECT.string() << "'" << std::endl;

			boost::this_thread::sleep( boost::posix_time::seconds( 3));
		}
		EXPECT_EQ( td.expected, output);
	}
}

int main( int argc, char **argv )
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();

	if (argc > 2)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		return 1;
	}
	else if (argc == 2)
	{
		selectedTestName = argv[1];
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}


