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
// iprocHandler class unit tests using google test framework (gTest)
//

#include "iprocHandler.hpp"
#include "connectionHandler.hpp"
#include "langbind/luaConfig.hpp"
#include "handlerConfig.hpp"
#include "config/configurationParser.hpp"
#include "testHandlerTemplates.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <list>
#include <fstream>
#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

using namespace _Wolframe;
using namespace iproc;

struct TestDescription
{
	const char* scriptfile;
	const char* datafile;
};

static const TestDescription testDescriptions[] =
{
	{"xml.lua", "test_IsoLatin1.xml"},
	{0,0}
};

struct TestConfiguration :public lua::Configuration
{
	TestConfiguration()
		:lua::Configuration( "iproc", "test-xml"){}
	TestConfiguration( const TestConfiguration& o) :lua::Configuration(o){}
	TestConfiguration( const char* scriptname, int bufferSizeInput, int bufferSizeOutput)
		:lua::Configuration( "iproc", "test-iproc")
	{
		boost::property_tree::ptree pt;
		pt.put("main", scriptname);
		pt.put("input_buffer", boost::lexical_cast<std::string>( bufferSizeInput));
		pt.put("output_buffer", boost::lexical_cast<std::string>( bufferSizeOutput));
		setCanonicalPathes( ".");
		if (!config::ConfigurationParser::parse<lua::Configuration>( *this, pt, "test"))
			throw std::logic_error( "Bad Configuration");
		setCanonicalPathes( ".");
	}
};

class XMLTestFixture : public ::testing::Test
{
protected:
	XMLTestFixture() {}
	virtual ~XMLTestFixture() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static std::string getDataFile( const char* name, const char* ext=0)
{
	boost::filesystem::path rt = boost::filesystem::current_path();
	std::string datafile( name);
	datafile.append( ext?ext:"");
	rt /= "data";
	rt /= datafile;
	return rt.string();
}

static bool readFile( const char* fn, std::string& out)
{
	char buf[ 1];
	FILE* gg = fopen( fn, "rb");
	if (gg)
	{
		std::cerr << "FILE " << fn << " can be opened" << std::endl;
		fclose( gg);
	}
	std::fstream ff( fn, std::ios::in | std::ios::binary);
	while (ff.read( buf, sizeof(buf)))
	{
		out.append( buf, ff.gcount());
	}
	out.append( buf, ff.gcount());
	if ((ff.rdstate() & std::ifstream::eofbit) != 0) return true;
	return false;
}

static void writeFile( const char* fn, const std::string& content)
{
	std::fstream ff( fn, std::ios::out | std::ios::binary);
	ff.write( content.c_str(), content.size());
}

TEST_F( XMLTestFixture, tests)
{
	unsigned int ti;
	for (ti=0; testDescriptions[ti].scriptfile; ti++)
	{
		enum {NofBufferSizes=8};
		static int BufferSize[ NofBufferSizes] = {2,3,4,5,6,7,8,127};

		std::string inputd;
		const char* ifnam = getDataFile( testDescriptions[ti].datafile).c_str();
		if (!readFile( ifnam, inputd)) throw std::runtime_error("could not read test input file");

		std::string expectd;
		const char* efnam = getDataFile( testDescriptions[ti].datafile, ".exp").c_str();
		readFile( efnam, expectd);

		net::LocalTCPendpoint  ep( "127.0.0.1", 12345);
		iproc::Connection* connection = 0;

		for (unsigned int ib=0; ib<NofBufferSizes; ib++)
		{
			for (unsigned int ob=0; ob<NofBufferSizes; ob++)
			{
				TestConfiguration config( testDescriptions[ti].scriptfile, BufferSize[ib], BufferSize[ob]);
				connection = new iproc::Connection( ep, &config);

				std::string prt_input;
				std::string prt_output;
				std::string prt_expect;

				prt_input.append( "run\r\n");
				prt_expect.append( "OK expecting command\r\n\r\n");

				prt_input.append( inputd);
				prt_expect.append( expectd);

				prt_input.append( ".\r\n");
				prt_expect.append( "\r\n\r\n.\r\nOK\r\n");
				prt_input.append( "quit\r\n");
				prt_expect.append( "BYE\r\n");

				char* in_start = const_cast<char*>(prt_input.c_str());
				char* in_end = const_cast<char*>(prt_input.c_str() + prt_input.size());

				EXPECT_EQ( 0, test::runTestIO( in_start, in_end, prt_output, *connection));
				ASSERT_EQ( prt_output, prt_expect);
#define _Wolframe_LOWLEVEL_DEBUG
#ifdef _Wolframe_LOWLEVEL_DEBUG
				unsigned int ii=0,nn=prt_output.size();
				for (;ii<nn && prt_output[ii]==prt_expect[ii]; ii++);
				if (ii != nn)
				{
					printf( "SIZE R=%lu,E=%lu,DIFF AT %u='%d %d %d %d|%d %d %d %d'\n",
						(unsigned long)prt_output.size(), (unsigned long)prt_expect.size(), ii,
						prt_output[ii-2],prt_output[ii-1],prt_output[ii-0],prt_output[ii+1],
						prt_expect[ii-2],prt_expect[ii-1],prt_expect[ii-0],prt_expect[ii+1]);
					boost::this_thread::sleep( boost::posix_time::seconds( 10 ));
				}
#endif
				std::string ext;
				ext.push_back( '_');
				ext.append( boost::lexical_cast<std::string>(BufferSize[ib]));
				ext.push_back( '_');
				ext.append( boost::lexical_cast<std::string>(BufferSize[ob]));
				ext.append( ".out");

				const char* ofnam = getDataFile( testDescriptions[ti].datafile, ext.c_str()).c_str();
				writeFile( ofnam, prt_output);
			}
		}
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


