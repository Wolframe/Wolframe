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
#include "langbind/directmapCommandEnvironment.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <ostream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void printIndent( std::ostream& out, size_t indent)
{
	while (indent--) out << "\t";
}

bool DirectmapCommandEnvironment::FormDescription::load( ddl::StructType& st) const
{
	st.clear();
	std::string error;
	if (!m_compiler->compileFile( m_path, st, error))
	{
		LOG_ERROR << "Failed to compile DDL: " << error;
		return false;
	}
	return true;
}

void DirectmapCommandEnvironment::print( std::ostream& out, size_t indent) const
{
	printIndent( out, indent);
	out << "Input Form " << m_inputformdescr.m_path << ":";
	m_inputform.print( out, indent+1);

	printIndent( out, indent);
	out << "Output Form " << m_outputformdescr.m_path << ":";
	m_outputform.print( out, indent+1);
}

bool DirectmapCommandEnvironment::test() const
{
	ddl::StructType ist,ost;
	if (!m_inputformdescr.load( ist))
	{
		LOG_ERROR << "Input form '" << m_inputformdescr.m_path << "' cannot be loaded";
		return false;
	}
	if (!m_outputformdescr.load( ost))
	{
		LOG_ERROR << "Output form '" << m_outputformdescr.m_path << "' cannot be loaded";
		return false;
	}
	return true;
}

bool DirectmapCommandEnvironment::check() const
{
	if (!boost::filesystem::exists( m_inputformdescr.m_path))
	{
		LOG_ERROR << "Input form file '" << m_inputformdescr.m_path << "' does not exist";
		return false;
	}
	if (!boost::filesystem::exists( m_outputformdescr.m_path))
	{
		LOG_ERROR << "Output form file '" << m_outputformdescr.m_path << "' does not exist";
		return false;
	}
	return true;
}

bool DirectmapCommandEnvironment::load()
{
	m_inputform = ddl::StructType();
	m_outputform = ddl::StructType();

	if (!m_inputformdescr.load( m_inputform))
	{
		LOG_ERROR << "Input form '" << m_inputformdescr.m_path << "' cannot be loaded";
		return false;
	}
	if (!m_outputformdescr.load( m_outputform))
	{
		LOG_ERROR << "Output form '" << m_outputformdescr.m_path << "' cannot be loaded";
		return false;
	}
	return true;
}


