/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file options.hpp
///\brief Interface for command line options of wolframec
#ifndef _WOLFRAME_CMDLINE_CLIENT_OPTIONS_HPP_INCLUDED
#define _WOLFRAME_CMDLINE_CLIENT_OPTIONS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include "session.hpp"

namespace _Wolframe {
namespace client {

class WolframecCommandLine
{
public:
	WolframecCommandLine( int argc, char **argv);
	~WolframecCommandLine(){}

	bool printhelp() const						{return m_printhelp;}
	bool printversion() const					{return m_printversion;}
	void print(std::ostream &) const;

	const Session::Configuration& config() const			{return m_config;}
	const std::string& request() const				{return m_request;}
	const std::vector<std::string>& document() const		{return m_document;}
	const std::string& uiformdirectory() const			{return m_uiformdirectory;}
	const std::string& outputfile() const				{return m_outputfile;}

private:
	bool m_printhelp;
	bool m_printversion;
	std::string m_helpstring;
	Session::Configuration m_config;
	std::string m_request;
	std::vector<std::string> m_document;
	std::string m_uiformdirectory;
	std::string m_outputfile;
};

}}//namespace
#endif

