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
///\brief Program using filters to map stdin to stdout

#include "langbind/appObjects.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace _Wolframe;

static void printUsage()
{
	std::cerr << "filter <inputfilter> <outputfilter>" << std::endl;
	std::cerr << "inputfilter :Name of the input filter plus an optional '/' plus buffer size" << std::endl;
	std::cerr << "outputfilter :Name of the output filter plus an optional '/' plus buffer size" << std::endl << std::endl;

	std::cerr << "   example: 'filter xml:textwolf/256 xml:textwolf:UTF-8/128'" << std::endl;
	std::cerr << "      input = xml:textwolf/256 = using textwolf XML with a buffer of 256 bytes" << std::endl;
	std::cerr << "      output = xml:textwolf:UTF-16/128 = using textwolf XML with UTF-8 encoding and a buffer of 128 bytes" << std::endl;
}

int main( int argc, char **argv )
{
	if (argc > 3)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 1;
	}
	else if (argc < 3)
	{
		std::cerr << "too many arguments passed to " << argv[0] << std::endl;
		printUsage();
		return 2;
	}
	std::size_t inputBufferSize = 256;
	std::size_t outputBufferSize = 256;
	std::string filternameIn( argv[1]);
	std::string filternameOut( argv[2]);
	const char* bp;
	bp = std::strchr( filternameIn.c_str(), '/');
	if (bp)
	{
		inputBufferSize = (std::size_t)atoi( bp+1);
		filternameIn.resize( bp-filternameIn.c_str());
	}
	bp = std::strchr( filternameOut.c_str(), '/');
	if (bp)
	{
		outputBufferSize = (std::size_t)atoi( bp+1);
		filternameOut.resize( bp-filternameOut.c_str());
	}
	langbind::Filter flt;

	if (boost::iequals( filternameIn, filternameOut))
	{
		if (!langbind::GlobalContext().getFilter( filternameIn.c_str(), flt))
		{
			std::cerr << "unknown filter " << filternameIn << std::endl;
			return 1;
		}
	}
	else
	{
		langbind::Filter in;
		langbind::Filter out;
		if (!langbind::GlobalContext().getFilter( filternameIn.c_str(), in))
		{
			std::cerr << "unknown input filter " << filternameIn << std::endl;
			return 1;
		}
		if (!langbind::GlobalContext().getFilter( filternameOut.c_str(), out))
		{
			std::cerr << "unknown output filter " << filternameOut << std::endl;
			return 2;
		}
		flt = langbind::Filter( in.inputfilter(), out.outputfilter());
	}
	char* inputBuffer = new char[ inputBufferSize];
	char* outputBuffer = new char[ outputBufferSize];
	char elementBuffer[ 4000];
	protocol::InputFilter::ElementType elementType;
	std::size_t elementBufferPos = 0;
	std::size_t taglevel=0;

	READ_INPUT:
	{
		if (flt.inputfilter().get()->gotEoD())
		{
			goto TERMINATE;
		}
		std::size_t pp = 0;
		while (pp < inputBufferSize && 1 == fread( inputBuffer+pp, sizeof(char), 1, stdin))
		{
			++pp;
		}
		flt.inputfilter().get()->protocolInput( inputBuffer, pp, pp < inputBufferSize);
		goto PROCESS_READ;
	}
	WRITE_OUTPUT:
	{
		std::size_t pp = 0, ee=flt.outputfilter().get()->pos();
		while (pp < ee && 1 == fwrite( outputBuffer+pp, sizeof(char), 1, stdout))
		{
			++pp;
		}
		flt.outputfilter().get()->init( outputBuffer, outputBufferSize);
		goto PROCESS_WRITE;
	}
	PROCESS_READ:
	{
		if (!flt.inputfilter().get()->getNext( &elementType, elementBuffer, sizeof(elementBuffer), &elementBufferPos))
		{
			switch (flt.inputfilter().get()->state())
			{
				case protocol::InputFilter::EndOfMessage:
				{
					if (flt.inputfilter().get()->size())
					{
						flt.inputfilter().get()->setState( protocol::InputFilter::Error, "Buffer too small");
						goto ERROR_READ;
					}
					else
					{
						goto READ_INPUT;
					}
				}
				case protocol::InputFilter::Error: goto ERROR_READ;
				case protocol::InputFilter::Open:
				{
					protocol::InputFilter* follow = flt.inputfilter()->createFollow();
					if (follow)
					{
						flt.inputfilter().reset( follow);
						goto PROCESS_READ;
					}
					else
					{
						goto ERROR_READ;
					}
				}
			}
		}
	}
	PROCESS_WRITE:
	{
		protocol::OutputFilter::ElementType tt = (protocol::OutputFilter::ElementType) elementType;
		if (!flt.outputfilter().get()->print( tt, elementBuffer, elementBufferPos))
		{
			switch (flt.outputfilter().get()->state())
			{
				case protocol::OutputFilter::EndOfBuffer:
				{
					goto WRITE_OUTPUT;
				}
				case protocol::OutputFilter::Error: goto ERROR_WRITE;
				case protocol::OutputFilter::Open:
				{
					protocol::OutputFilter* follow = flt.outputfilter()->createFollow();
					if (follow)
					{
						flt.outputfilter().reset( follow);
						goto PROCESS_WRITE;
					}
					else
					{
						goto ERROR_WRITE;
					}
				}

			}
		}
		else
		{
			elementBufferPos = 0;
		}
		if (elementType == protocol::InputFilter::OpenTag)
		{
			++taglevel;
		}
		else if (elementType == protocol::InputFilter::CloseTag)
		{
			--taglevel;
			if (taglevel == 0) goto TERMINATE;
		}
		goto PROCESS_READ;
	}
	ERROR_READ:
	{
		std::cerr << "Error in input: '" << flt.inputfilter().get()->getError() << "'" << std::endl;
		goto TERMINATE;
	}
	ERROR_WRITE:
		std::cerr << "Error in output: '" << flt.outputfilter().get()->getError() << "'" << std::endl;
		goto TERMINATE;
	TERMINATE:
	{
		std::size_t pp = 0, ee=flt.outputfilter().get()->pos();
		while (pp < ee && 1 == fwrite( outputBuffer+pp, sizeof(char), 1, stdout)) ++pp;
		delete [] inputBuffer;
		delete [] outputBuffer;
	}
	return 0;
}


