/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// tprocHandler class unit tests using google test framework (gTest)
//

#include "tprocHandler.hpp"
#include "connectionHandler.hpp"
#include "handlerConfig.hpp"
#include "moduleInterface.hpp"
#include "config/ConfigurationTree.hpp"
#include "testHandlerTemplates.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <gtest/gtest.h>
#include <boost/thread/thread.hpp>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace _Wolframe;
using namespace _Wolframe::tproc;

class TestConfiguration :public Configuration
{
public:
	TestConfiguration( const TestConfiguration& o)
		:Configuration(o)
	{}

	TestConfiguration( std::size_t ib, std::size_t ob, const std::string& config)
	{
		m_data.input_bufsize = ib;
		m_data.output_bufsize = ob;

		std::istringstream cfgreader( config);
		boost::property_tree::ptree pt;
		boost::property_tree::read_info( cfgreader, pt);

		module::ModulesDirectory modules;
		if (!parse( config::ConfigurationTree(pt), std::string("tproc"), &modules)) throw std::runtime_error( "error in configuration");
		setCanonicalPathes( (boost::filesystem::current_path() / "temp" / "example.txt").string());
	}
};

struct TestDescription
{
	std::string input;
	std::string expected;
	std::string config;
	std::string requires;

	TestDescription(){}
	TestDescription( const TestDescription& o)
		:input(o.input),expected(o.expected),config(o.config),requires(o.requires)
	{}
};

static const char* check_flag( const std::string& flag)
{
if (boost::iequals( flag, "DISABLED")) return "DISABLED ";
if (boost::starts_with( flag, "DISABLED "))
{
	std::vector<std::string> platforms;
	boost::split( platforms, flag, boost::is_any_of("\t "));
	std::vector<std::string>::const_iterator ii = platforms.begin(),ee = platforms.end();
	for (++ii; ii != ee; ++ii)
	{
#if PLATFORM_NETBSD
		if (boost::iequals( *ii, "NETBSD")) return "DISABLED ON PLATFORM NETBSD ";
#endif
	}
}
#if !(WITH_LUA)
	if (boost::iequals( flag, "LUA")) return "WITH_LUA=1 ";
#endif
#if !(WITH_SSL)
	if (boost::iequals( flag, "SSL")) return "WITH_SSL=1 ";
#endif
#if !(WITH_QT)
	if (boost::iequals( flag, "QT")) return "WITH_QT=1 ";
#endif
#if !(WITH_PAM)
	if (boost::iequals( flag, "PAM")) return "WITH_PAM=1 ";
#endif
#if !(WITH_SASL)
	if (boost::iequals( flag, "SASL")) return "WITH_SASL=1 ";
#endif
#if !(WITH_SQLITE3)
	if (boost::iequals( flag, "SQLITE3")) return "WITH_SQLITE3=1 ";
#endif
#if !(WITH_LOCAL_SQLITE3)
	if (boost::iequals( flag, "LOCAL_SQLITE3")) return "WITH_LOCAL_SQLITE3=1 ";
#endif
#if !(WITH_PGSQL)
	if (boost::iequals( flag, "PGSQL")) return "WITH_PGSQL=1 ";
#endif
#if !(WITH_LIBXML2)
	if (boost::iequals( flag, "LIBXML2")) return "WITH_LIBXML2=1 ";
#endif
#if !(WITH_XMLLITE)
	if (boost::iequals( flag, "XMLLITE")) return "WITH_XMLLITE=1 ";
#endif
#if !(WITH_MSXML)
	if (boost::iequals( flag, "MSXML")) return "WITH_MSXML=1 ";
#endif
#if !(WITH_LIBXSLT)
	if (boost::iequals( flag, "LIBXSLT")) return "WITH_LIBXSLT=1 ";
#endif
	return "";
}

static void readFile( const std::string& pt, std::vector<std::string>& hdr, std::vector<std::string>& out, std::string& requires)
{
	std::string element;
	char chb;
	std::fstream infh;
	infh.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	infh.open( pt.c_str(), std::ios::in | std::ios::binary);
	std::string splitstr;
	std::string::const_iterator splititr;
	enum
	{
		PARSE_HDR,
		PARSE_OUT,
		PARSE_NOT
	}
	type = PARSE_HDR;

	while (infh.read( &chb, sizeof(chb)))
	{
		if (chb == '\r' || chb == '\n') break;
		splitstr.push_back( chb);
	}
	splititr = splitstr.begin();
	if (splititr == splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. no split tag defined at file start");
	}
	while (infh.read( &chb, sizeof(chb)))
	{
		if (chb != '\r' && chb != '\n') break;
	}
	do
	{
		if (chb != *splititr) break;
		++splititr;
	}
	while (splititr != splitstr.end() && infh.read( &chb, sizeof(chb)));

	if (splititr != splitstr.end())
	{
		throw std::runtime_error( "illegal test definition file. header expected after split tag definition");
	}
	splititr = splitstr.begin();

	while (infh.read( &chb, sizeof(chb)))
	{
		if (type == PARSE_HDR)
		{
			if (chb == '\n')
			{
				std::string tag( std::string( element.c_str(), element.size()));
				boost::trim( tag);
				if (boost::iequals( tag, "end"))
				{
					infh.close();
					return;
				}
				else if (boost::starts_with( tag, "requires:"))
				{
					std::size_t nn = std::strlen("requires:");
					std::string flagname( std::string( tag.c_str()+nn, tag.size()-nn));
					boost::trim( flagname);
					requires.append( check_flag( flagname));
					type = PARSE_NOT;
					splititr = splitstr.begin();
				}
				else
				{
					hdr.push_back( tag);
					element.clear();
					type = PARSE_OUT;
				}
			}
			else
			{
				element.push_back( chb);
			}
		}
		else if (type == PARSE_OUT)
		{
			element.push_back( chb);
			if (chb == *splititr)
			{
				++splititr;
				if (splititr == splitstr.end())
				{
					out.push_back( std::string( element.c_str(), element.size() - splitstr.size()));
					element.clear();
					type = PARSE_HDR;
					splititr = splitstr.begin();
				}
			}
			else
			{
				splititr = splitstr.begin();
			}
		}
		else if (type == PARSE_NOT)
		{
			if (chb == *splititr)
			{
				++splititr;
				if (splititr == splitstr.end())
				{
					element.clear();
					type = PARSE_HDR;
					splititr = splitstr.begin();
				}
			}
			else if (chb > ' ' || chb < 0)
			{
				throw std::runtime_error( "illegal test definition file. tag definition expected");
			}
			else
			{
				splititr = splitstr.begin();
			}
		}
	}
	throw std::runtime_error( "no end tag at end of file");
}

static void writeFile( const std::string& pt, const std::string& content)
{
	std::fstream ff( pt.c_str(), std::ios::out | std::ios::binary);
	ff.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	ff.write( content.c_str(), content.size());
}


static const TestDescription getTestDescription( const std::string& pt)
{
	TestDescription rt;
	std::vector<std::string> header;
	std::vector<std::string> content;
	readFile( pt, header, content, rt.requires);

	std::vector<std::string>::const_iterator hi=header.begin();
	std::vector<std::string>::const_iterator itr=content.begin(),end=content.end();

	for (;itr != end; ++itr,++hi)
	{
		if (boost::iequals( *hi, "input"))
		{
			rt.input.append( *itr);
		}
		else if (boost::iequals( *hi, "output"))
		{
			rt.expected.append( *itr);
		}
		else if (boost::iequals( *hi, "config"))
		{
			rt.config.append( *itr);
		}
		else if (boost::starts_with( *hi, "file:"))
		{
			std::string filename( hi->c_str()+std::strlen("file:"));
			boost::trim( filename);
			boost::filesystem::path fn( boost::filesystem::current_path() / "temp" / filename);
			writeFile( fn.string(), *itr);
		}
	}
	return rt;
}

class TProcHandlerTest : public ::testing::Test
{
protected:
	TProcHandlerTest() {}
	virtual ~TProcHandlerTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

class TProcHandlerTestInstance
{
private:
	net::LocalTCPendpoint ep;
	tproc::Connection* m_connection;
	TestConfiguration m_config;
	std::string m_input;
	std::string m_output;
	std::string m_expected;
	std::size_t m_inputBufferSize;
	std::size_t m_outputBufferSize;

	enum
	{
		EoDBufferSize=4,
		MinOutBufferSize=24
	};

	static unsigned int buffersizeFactor( const std::string& inp)
	{
		unsigned ii=0,nn=inp.size();
		std::string::const_iterator itr=inp.begin(),end=inp.end();
		for (;itr != end; ++itr)
		{
			if (*itr)
			{
				++ii;
			}
		}
		if (end - itr <= nn/4) return 5;
		if (end - itr <= nn/3) return 4;
		if (end - itr <= nn/2) return 3;
		return 1;
	}

public:
	TProcHandlerTestInstance( const TestDescription& descr, std::size_t ib, std::size_t ob)
		:ep( "127.0.0.1", 12345)
		,m_connection(0)
		,m_config( ib + EoDBufferSize, buffersizeFactor( descr.input) * (ob + MinOutBufferSize), descr.config)
		,m_input( descr.input)
		,m_expected( descr.expected)
		,m_inputBufferSize(ib)
		,m_outputBufferSize(ob)
	{
		m_connection = new tproc::Connection( ep, &m_config);
	}

	~TProcHandlerTestInstance()
	{
		delete m_connection;
	}

	const std::string& input() const {return m_input;}
	const std::string& output() const {return m_output;}
	const std::string& expected() const {return m_expected;}

	int run()
	{
		char* itr = const_cast<char*>( m_input.c_str());
		m_output.clear();
		return test::runTestIO( itr, itr+m_input.size(), m_output, *m_connection);
	}
};

static std::string selectedTestName;

TEST_F( TProcHandlerTest, tests)
{
	enum {NOF_IB=6,NOF_OB=4};
	std::size_t ib[] = {127,1,2,3,5,7};
	std::size_t ob[] = {127,1,2,5};
	std::vector<std::string> tests;

	boost::filesystem::recursive_directory_iterator ditr( boost::filesystem::current_path() / "data"), dend;
	if (selectedTestName.size())
	{
		std::cerr << "executing tests matching '" << selectedTestName << "'" << std::endl;
	}
	for (; ditr != dend; ++ditr)
	{
		if (boost::iequals( boost::filesystem::extension( *ditr), ".tst"))
		{
			if (selectedTestName.size())
			{
				if (std::search( ditr->path().string().begin(), ditr->path().string().end(), selectedTestName.begin(), selectedTestName.end()) != ditr->path().string().end())
				{
					std::cerr << "selected test '" << *ditr << "'" << std::endl;
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

	std::vector<std::string>::const_iterator itr=tests.begin(),end=tests.end();
	for (; itr != end; ++itr)
	{
		boost::filesystem::remove_all( boost::filesystem::current_path() / "temp" );
		boost::filesystem::create_directory( boost::filesystem::current_path() / "temp");

		const TestDescription td = getTestDescription( *itr);
		if (td.requires.size())
		{
			std::cerr << "skipping test '" << *itr << "' ( is " << td.requires << ")" << std::endl;
			continue;
		}
		std::cerr << "processing test '" << *itr << "'" << std::endl;
		for (int ii=0; ii<NOF_IB; ii++)
		{
			for (int oo=0; oo<NOF_OB; oo++)
			{
				TProcHandlerTestInstance test( td, ib[ii], ob[oo]);
				int trt = test.run();
				if (trt != 0) boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
				EXPECT_EQ( 0, trt);
				if (test.expected() != test.output()) boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
				EXPECT_EQ( test.expected(), test.output());
			}
		}
	}
}

int main( int argc, char **argv )
{
	int gtest_ARGC = 1;
	char* gtest_ARGV[2] = {argv[0], 0};
	if (argc > 2)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		return 1;
	}
	else if (argc == 2)
	{
		selectedTestName = argv[1];
	}
	::testing::InitGoogleTest( &gtest_ARGC, gtest_ARGV );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}

