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
///\file wolfilterOptions.hpp
///\brief Interface for the options of a wolfilter call
#ifndef _Wolframe_WOLFILTER_OPTIONS_HPP_INCLUDED
#define _Wolframe_WOLFILTER_OPTIONS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>

namespace _Wolframe {
namespace config {

class WolfilterOptions
{
public:
	WolfilterOptions( int argc, const char** argv);
	WolfilterOptions( const WolfilterOptions& o)
		:m_help(o.m_help)
		,m_inputfile(o.m_inputfile)
		,m_scripts(o.m_scripts)
		,m_modules(o.m_modules){}
	~WolfilterOptions(){}

	bool help() const					{return m_help;}
	bool printversion() const				{return m_printversion;}
	const std::string& inputfile() const			{return m_inputfile;}
	const std::vector<std::string>& scripts() const		{return m_scripts;}
	const std::vector<std::string>& modules() const		{return m_modules;}
	const std::string& cmd() const				{return m_cmd;}
	const std::string& inputfilter() const			{return m_inputfilter;}
	const std::string& outputfilter() const			{return m_outputfilter;}

	void print(std::ostream &) const;

private:
	bool m_help;
	bool m_printversion;
	std::string m_inputfile;
	std::vector<std::string> m_scripts;
	std::vector<std::string> m_modules;
	std::string m_cmd;
	std::string m_inputfilter;
	std::string m_outputfilter;
	std::string m_helpstring;
};

}}//namespace
#endif

