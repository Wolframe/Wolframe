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
#ifndef _Wolframe_TEST_UTILITIES_HPP_INCLUDED
#define _Wolframe_TEST_UTILITIES_HPP_INCLUDED
///\file tests/testUtils.hpp
///\brief Some common code of tests (not in an own object file because of test program build issues)
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdio>

namespace _Wolframe {
namespace wtest {

struct Data
{
	Data( const char* testname, const char* inputfilename, const char* argv0)
	{
		name = testname;
		inputf = getDataFile( inputfilename, "data", 0, argv0);
		if (!readFile( inputf.c_str(), input)) throw std::runtime_error("could not read test input file");

		expectedf = getDataFile( testname, "must", ".txt", argv0);
		readFile( expectedf.c_str(), expected);

		resultf = getDataFile( testname, "result", ".txt", argv0);
		std::cerr << "in case of error the output is written to '" << resultf << "'" << std::endl;
	}

	std::string name;
	std::string input;
	std::string expected;

	std::string inputf;
	std::string resultf;
	std::string expectedf;

	bool check( const std::string& result) const
	{
		unsigned int ii=0,nn=result.size();
		for (;ii<nn && result[ii]==expected[ii]; ii++);
		if (ii != nn)
		{
			// write output to file to check the result in case of an error
			writeFile( resultf.c_str(), result);
			enum {Diffsize=30};
			std::string oo( result.c_str()+ii, ((std::size_t)Diffsize)>result.size()?result.size():(std::size_t)Diffsize);
			for (std::size_t kk=0; kk<oo.size(); kk++) if (oo[kk] < ' ' || (unsigned char)oo[kk] > 128) oo[kk] = '_';
			std::string mm( expected.c_str()+ii, ((std::size_t)Diffsize)>expected.size()?expected.size():(std::size_t)Diffsize);
			for (unsigned int kk=0; kk<mm.size(); kk++) if (mm[kk] < ' ' || (unsigned char)mm[kk] > 128) mm[kk] = '_';
			if (mm.size() == Diffsize) mm.append( "...");
			if (oo.size() == Diffsize) oo.append( "...");
			if (oo.size() >= Diffsize && mm.size() >= Diffsize)
			{
				printf( "Input \"%s\" Expected \"%s\" Result \"%s\"\n", inputf.c_str(), expectedf.c_str(), resultf.c_str());
				printf( "TEST %s SIZE R=%lu,E=%lu,DIFF AT %u='%d %d %d %d|%d %d %d %d' \"%s\" \"%s\"\n",
				name.c_str(),
				(unsigned long)result.size(), (unsigned long)expected.size(), ii,
				result[ii-2],result[ii-1],result[ii-0],result[ii+1],
				expected[ii-2],expected[ii-1],expected[ii-0],expected[ii+1],
				oo.c_str(), mm.c_str());
			}
			boost::this_thread::sleep( boost::posix_time::seconds( 5 ));
			return false;
		}
		return true;
	}

	static std::string getDataFile( const char* name, const char* type, const char* ext, const char* argv0)
	{
		static boost::filesystem::path testdir = boost::filesystem::system_complete( argv0).parent_path();
		boost::filesystem::path rt = testdir;
		std::string datafile( name);
		datafile.append( ext?ext:"");
		rt = testdir / type / datafile;
		return rt.string();
	}

	static bool readFile( const char* fn, std::string& out)
	{
		char buf;
		std::fstream ff;
		ff.open( fn, std::ios::in | std::ios::binary);
		while (ff.read( &buf, sizeof(buf)))
		{
			out.push_back( buf);
		}
		bool rt = ((ff.rdstate() & std::ifstream::eofbit) != 0);
		ff.close();
		return rt;
	}

	static void writeFile( const char* fn, const std::string& content)
	{
		std::fstream ff( fn, std::ios::out | std::ios::binary);
		ff.write( content.c_str(), content.size());
	}

	static void createDataDir( const char* type, const char* argv0)
	{
		static boost::filesystem::path testdir = boost::filesystem::system_complete( argv0).parent_path();
		boost::filesystem::path pt = testdir / type;
		boost::filesystem::create_directory( pt);
	}
};
}}
#endif
