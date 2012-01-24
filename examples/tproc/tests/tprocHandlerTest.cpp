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
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

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
		setCanonicalPathes( (boost::filesystem::current_path() / "temp").string());
	}
};

struct TestDescription
{
	std::string input;
	std::string expected;
	std::string config;
};

static void readFile( const boost::filesystem::path& pt, std::vector<std::string>& hdr, std::vector<std::string>& out)
{
	std::string element;
	char chb;
	std::fstream infh;
	infh.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	infh.open( pt.string().c_str(), std::ios::in | std::ios::binary);
	std::string splitstr;
	std::string::const_iterator splititr;
	enum
	{
		PARSE_HDR,
		PARSE_OUT
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
				}
			}
			else
			{
				splititr = splitstr.begin();
			}
		}
	}
	throw std::runtime_error( "no end tag at end of file");
}

static void writeFile( const boost::filesystem::path& pt, const std::string& content)
{
	std::fstream ff( pt.string().c_str(), std::ios::out | std::ios::binary);
	ff.exceptions( std::ifstream::failbit | std::ifstream::badbit);
	ff.write( content.c_str(), content.size());
}

static const TestDescription getTestDescription( const boost::filesystem::path& pt)
{
	TestDescription rt;
	std::vector<std::string> header;
	std::vector<std::string> content;
	readFile( pt, header, content);

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
			std::string filename( hi->c_str()+sizeof("file:"));
			boost::trim( filename);
			boost::filesystem::path fn( boost::filesystem::current_path() / "temp" / filename);
			writeFile( fn, *itr);
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
		MinOutBufferSize=16
	};
public:
	TProcHandlerTestInstance( const TestDescription& descr, std::size_t ib, std::size_t ob)
		:ep( "127.0.0.1", 12345)
		,m_connection(0)
		,m_config( ib + EoDBufferSize, ob + MinOutBufferSize, descr.config)
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

TEST_F( TProcHandlerTest, tests)
{
	enum {NOF_IB=8,NOF_OB=3};
	std::size_t ib[ NOF_IB] = {1,2,3,5,11,13,23,127};
	std::size_t ob[ NOF_OB] = {1,2,3};

	boost::filesystem::recursive_directory_iterator itr( boost::filesystem::current_path() / "data"), end;

	for (; itr != end; ++itr)
	{
		if (boost::iequals( boost::filesystem::extension( *itr), ".txt"))
		{
			std::cerr << "Processing file '" << *itr << "'" << std::endl;

			boost::filesystem::remove_all( boost::filesystem::current_path() / "temp" );
			boost::filesystem::create_directory( boost::filesystem::current_path() / "temp");

			TestDescription td = getTestDescription( *itr);
			for (int ii=0; ii<NOF_IB; ii++)
			{
				for (int oo=0; oo<NOF_OB; oo++)
				{
					TProcHandlerTestInstance test( td, ib[ii], ob[oo]);
					EXPECT_EQ( 0, test.run());
					EXPECT_EQ( test.expected(), test.output());
				}
			}
		}
		else
		{
			std::cerr << "Ignoring file '" << *itr << "'" << std::endl;
		}
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	_Wolframe::log::LogBackend::instance().setConsoleLevel( _Wolframe::log::LogLevel::LOGLEVEL_INFO );
	return RUN_ALL_TESTS();
}

