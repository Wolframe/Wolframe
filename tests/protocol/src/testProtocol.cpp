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
///\file testProtocol.cpp
///\brief Test program for the wolframe main protocol

#include "utils/fileUtils.hpp"
#include "processor/procProvider.hpp"
#include "module/moduleDirectory.hpp"
#include "mainConnectionHandler.hpp"
#include "logger-v1.hpp"
#include "gtest/gtest.h"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>

using namespace _Wolframe;

/// \brief Pseudo random number generator depending on current day (for reproducability)
class Random
{
public:
	Random()
	{
		time_t nowtime;
		struct tm* now;

		::time ( &nowtime);	//init time now
		now = ::localtime( &nowtime);

		m_cur = ((now->tm_year+1) * (now->tm_mon+100) * (now->tm_mday+1)) * KnuthIntegerHashFactor;
	}

	unsigned int get( unsigned int min_, unsigned int max_)
	{
		m_cur = (m_cur+123) * KnuthIntegerHashFactor;
		unsigned int iv = max_ - min_;
		return iv?((m_cur % iv) + min_):min_;
	}

	unsigned int seed() const
	{
		return m_cur;
	}

private:
	enum {KnuthIntegerHashFactor=2654435761U};
	unsigned int m_cur;
};

static int g_gtest_ARGC = 0;
static char* g_gtest_ARGV[2] = {0, 0};
static boost::filesystem::path g_testdir;
static std::string g_selectedTestName;
static Random g_random;

struct BufferStruct
{
	boost::shared_ptr<char> mem;
	char* inbuf;
	char* outbuf;
	std::size_t insize;
	std::size_t outsize;

	BufferStruct( std::size_t ib, std::size_t ob)
		:mem( (char*)std::calloc( ib+ob, 1), std::free),insize(ib),outsize(ob)
	{
		inbuf = mem.get();
		outbuf = mem.get()+ib;
	}
};

static void readInput( cmdbind::CommandHandler& cmdhandler, char* buf, unsigned int bufsize, std::istream& is)
{
	unsigned int readsize = g_random.get( 1, bufsize);
	std::size_t pp = 0;
	while (pp < readsize && !is.eof())
	{
		is.read( buf+pp, sizeof(char));
		if (!is.eof()) ++pp;
	}
	if (pp == 0 && is.eof())
	{
		throw std::runtime_error("unexpected end of file");
	}
	cmdhandler.putInput( buf, pp);
}

static std::string normalizeOutputCRLF( const std::string& output, const std::string& expected_)
{
	std::string::const_iterator ei = expected_.begin(), ee = expected_.end();
	std::string::const_iterator oi = output.begin(), oe = output.end();
	std::string resultstr;

	for (; ei != ee && oi != oe; ++oi,++ei)
	{
		if (*ei == *oi)
		{
			resultstr.push_back( *ei);
		}
		else if (*ei == '\r' && *oi == '\n')
		{
			++ei;
			if (*ei == '\n')
			{
				resultstr.push_back( '\r');
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
		else if (*ei == '\n' && *oi == '\r')
		{
			++oi;
			if (*oi == '\n')
			{
				resultstr.push_back( '\n');
			}
			else
			{
				break;
			}
		}
	}
	if (oi == oe && ei == ee)
	{
		return resultstr;
	}
	else
	{
		return output;
	}
}

static void processCommandHandlerSTM( cmdbind::CommandHandler& cmdhandler, BufferStruct& buf, std::istream& is, std::ostream& os)
{
	const void* cmdh_output;
	std::size_t cmdh_outputsize;
	const char* error;
	cmdhandler.setInputBuffer( buf.inbuf, buf.insize);
	cmdhandler.setOutputBuffer( buf.outbuf, buf.outsize, 0);

	for (;;) switch (cmdhandler.nextOperation())
	{
		case cmdbind::CommandHandler::READ:
			readInput( cmdhandler, buf.inbuf, buf.insize, is);
			continue;

		case cmdbind::CommandHandler::WRITE:
			cmdhandler.getOutput( cmdh_output, cmdh_outputsize);
			os << std::string( (const char*)cmdh_output, cmdh_outputsize);
			continue;

		case cmdbind::CommandHandler::CLOSE:
			error = cmdhandler.lastError();
			if (error)
			{ 
				std::ostringstream msg;
				msg << "error process command handler: " << error;
				throw std::runtime_error( msg.str());
			}
			return;
	}
}

static void processCommandHandler( cmdbind::CommandHandler& cmdhandler, const std::string& input, std::size_t ibsize, std::string& output, std::size_t obsize)
{
	BufferStruct buf( ibsize, obsize);
	std::istringstream is( input);
	std::ostringstream os;
	processCommandHandlerSTM( cmdhandler, buf, is, os);

	// Check if there is unconsumed input left (must not happen):
	bool end = false;
	while (!end)
	{
		char ch = 0;
		is.read( &ch, sizeof(char));
		if ((unsigned char)ch > 32)
		{
			throw std::runtime_error( "unconsumed input left");
		}
		end = is.eof();
	}
	output = os.str();
}

struct GlobalContext
{
	static std::vector<std::string> configModules( const types::PropertyTree& cfg, const std::string& refpath)
	{
		std::vector<std::string> rt;
		types::PropertyTree::Node module_section = cfg.root().getChild( "LoadModules");

		types::PropertyTree::Node::const_iterator mi = module_section.begin(), me = module_section.end();
		for (; mi != me; ++mi)
		{
			if (boost::algorithm::iequals( mi->first, "module"))
			{
				rt.push_back( utils::getCanonicalPath( mi->second.data().string(), refpath));
			}
		}
		return rt;
	}

	explicit GlobalContext( const std::string& configfile)
		:m_databaseProvider(0)
		,m_processorProvider(0)
		,m_execContext(0)
	{
		m_config = utils::readPropertyTreeFile( configfile);
		m_referencePath = boost::filesystem::path( configfile ).branch_path().string();
		m_modules = configModules( m_config, m_referencePath);

		std::list<std::string> modfiles;
		std::copy( m_modules.begin(), m_modules.end(), std::back_inserter( modfiles));
		if (!LoadModules( m_modulesDirectory, modfiles))
		{
			throw std::runtime_error( "Modules could not be loaded");
		}

		if (!m_dbProviderConfig.parse( m_config.root().getChild( "database"), "", &m_modulesDirectory))
		{
			throw std::runtime_error( "Database provider configuration could not be created from command line");
		}
		m_dbProviderConfig.setCanonicalPathes( m_referencePath);
		if (!m_dbProviderConfig.check())
		{
			throw std::runtime_error( "error in command line. failed to setup a valid database provider configuration");
		}

		if (!m_procProviderConfig.parse( m_config.root().getChild( "Processor"), "", &m_modulesDirectory))
		{
			throw std::runtime_error( "Error in processor provider configuration");
		}
		m_procProviderConfig.setCanonicalPathes( m_referencePath);
		if (!m_procProviderConfig.check())
		{
			throw std::runtime_error( "Invalid processor provider configuration");
		}

		// Load the modules, scripts, etc. defined in the command line into the global context:
		m_databaseProvider = new db::DatabaseProvider( &m_dbProviderConfig, &m_modulesDirectory);
		m_processorProvider = new proc::ProcessorProvider( &m_procProviderConfig, &m_modulesDirectory, &m_programLibrary);
		m_execContext = new proc::ExecContext( m_processorProvider);
	}

	~GlobalContext()
	{
		if (m_databaseProvider) delete m_databaseProvider;
		if (m_processorProvider) delete m_processorProvider;
		if (m_execContext) delete m_execContext;
	}

	void resetExecContext()
	{
		if (m_execContext) delete m_execContext;
		m_execContext = new proc::ExecContext( m_processorProvider);
	}

	proc::ExecContext* execContext()
	{
		return m_execContext;
	}

private:
	types::PropertyTree m_config;
	proc::ProcProviderConfig m_procProviderConfig;
	db::DBproviderConfig m_dbProviderConfig;

	std::string m_referencePath;
	std::vector<std::string> m_modules;
	module::ModulesDirectory m_modulesDirectory;

	prgbind::ProgramLibrary m_programLibrary;
	db::DatabaseProvider* m_databaseProvider;
	proc::ProcessorProvider* m_processorProvider;
	proc::ExecContext* m_execContext;
};


class MainProtocolTest
	:public ::testing::Test
{
protected:
	MainProtocolTest() {}
	virtual ~MainProtocolTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F( MainProtocolTest, tests)
{
	std::vector<std::string> tests;
	std::size_t testno;
	boost::filesystem::path outputdir( g_testdir / ".." / "output");
	boost::filesystem::path configfile( g_testdir / ".." / "config" / "wolframe.conf");

	GlobalContext processingContext( configfile.string());

	// [1] Selecting tests to execute:
	if (g_selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << g_selectedTestName << "'" << std::endl;
	}
	boost::filesystem::recursive_directory_iterator ditr( g_testdir / ".." / "data" ), dend;
	for (; ditr != dend; ++ditr)
	{
		std::string filename = utils::resolvePath( ditr->path().string());
		if (boost::iequals( boost::filesystem::extension( *ditr), ".inp"))
		{
			std::string testname = utils::getFileStem( filename);
			if (g_selectedTestName.size())
			{
				if (std::strstr( testname.c_str(), g_selectedTestName.c_str()))
				{
					std::cerr << "selected test '" << testname << "'" << std::endl;
					tests.push_back( filename);
				}
			}
			else
			{
				tests.push_back( filename);
			}
		}
		else if (!boost::filesystem::is_directory( *ditr))
		{
			std::cerr << "ignoring file '" << filename << "'" << std::endl;
		}
	}
	std::sort( tests.begin(), tests.end());
	std::cerr << "Outputs of failed tests are written to '" << outputdir.string() << "'" << std::endl;

	// [2] Execute tests:
	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (testno=1; itr != end; ++itr,++testno)
	{
		std::string testname = utils::getFileStem( *itr);
		std::string testdir = utils::getParentPath( *itr);
		std::string inpfile = testdir + "/" + testname + ".inp";
		std::string expectfile = testdir + "/" + testname + ".res";

		// [2.1] Process test:
		unsigned int seed = g_random.seed();
		std::cerr << "processing test '" << testname << "'" << std::endl;

		// [2.1.1] Read test files needed:
		std::string input = utils::readBinaryFileContent( inpfile);
		std::string expected;
		try
		{
			expected = utils::readBinaryFileContent( expectfile);
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << "failed to read expect file '" << expectfile << "': " << e.what() << std::endl; 
		}

		// [2.1.2] Define buffer sizes:
		enum {ibarsize=10, obarsize=10};
		std::size_t ibar[ibarsize];
		std::size_t obar[obarsize];
		unsigned int ii = 0;
		for (; ii+1<ibarsize; ii++)
		{
			ibar[ii] = g_random.get( 4, 4 + (ii+1) * 4);
		}
		ibar[ii] = g_random.get( 4, 16000);

		unsigned int oo = 0;
		for (; oo+1<obarsize; oo++)
		{
			obar[oo] = g_random.get( 16, 16 + (oo+1)*4);
		}
		obar[oo] = g_random.get( 16, 16000);

		// [2.1.3] Run test with various buffer size combinations:
		for (ii=0; ii<ibarsize; ii++)
		{
			for (oo=0; oo<obarsize; oo++)
			{
				proc::MainCommandHandler cmdhandler;
				processingContext.resetExecContext();
				cmdhandler.setExecContext( processingContext.execContext());

				std::string output;
				processCommandHandler( cmdhandler, input, ibar[ii], output, obar[oo]);
				output = normalizeOutputCRLF( output, expected);

				if (expected != output)
				{
					std::cerr << "test failed (seed = " << seed << ", input buffer size = " << ibar[ii] << ", output buffer size = " << obar[oo] << ")" << std::endl;
					boost::filesystem::path outputdumpfile( outputdir / (testname + ".res"));
					utils::writeFile( outputdumpfile.string(), output);
					boost::this_thread::sleep( boost::posix_time::seconds( 3));
				}
				EXPECT_EQ( expected, output);
			}
		}
	}
}


int main( int argc, char **argv)
{
	g_gtest_ARGC = 1;
	g_gtest_ARGV[0] = argv[0];
	g_testdir = boost::filesystem::system_complete( argv[0]).parent_path();

	if (argc >= 2)
	{
		if (std::strcmp( argv[1], "-h") == 0 || std::strcmp( argv[1], "--help") == 0)
		{
			std::cerr << argv[0] << " (no arguments)" << std::endl;
			return 0;
		}
		else if (argc == 2)
		{
			g_selectedTestName = argv[ 1];
		}
		else
		{
			std::cerr << "Too many arguments (expected no arguments)" << std::endl;
			return 1;
		}
	}
	::testing::InitGoogleTest( &g_gtest_ARGC, g_gtest_ARGV);
	return RUN_ALL_TESTS();
}


