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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file ddl_compilerInterface.cpp
///\brief implementation of common methods of DDL compilers
#include "ddl/compilerInterface.hpp"
#include "utils/miscUtils.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>

using namespace _Wolframe;
using namespace ddl;

StructType CompilerInterface::compileFile( const std::string& filename) const
{
	std::ostringstream src;
	std::ifstream inFile( filename.c_str());
	try
	{
		inFile.exceptions( std::ifstream::failbit | std::ifstream::badbit);

		while (inFile)
		{
			std::string ln;
			if (inFile.eof()) break;
			std::getline( inFile, ln);
			src << ln << "\n";
		}
		return compile( src.str());
	}
	catch (const std::ifstream::failure& e)
	{
		if (!(inFile.rdstate() & std::ifstream::eofbit))
		{
			// ... puzzle: I try to mask the EOF exception, but it is still thrown. Maybe because of 'getline' ?
			std::ostringstream msg;
			msg << "error '" << e.what() << "' reading file '" << filename << "'" << std::endl;
			throw std::runtime_error( msg.str());
		}
		else
		{
			return compile( src.str());
		}
	}
	catch (const std::exception& e)
	{
		std::ostringstream msg;
		msg << "error '" << e.what() << "' loading file '" << filename << "'" << std::endl;
		throw std::runtime_error( msg.str());
	}
}

