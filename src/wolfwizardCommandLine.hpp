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
///\file wolfwizardCommandLine.hpp
///\brief Interface for the wolfwizard call command line
#ifndef _Wolframe_WOLFWIZARD_COMMANDLINE_HPP_INCLUDED
#define _Wolframe_WOLFWIZARD_COMMANDLINE_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include "module/moduleDirectory.hpp"
#include "processor/procProvider.hpp"
#include "types/propertyTree.hpp"

namespace _Wolframe {
namespace config {

class WolfwizardCommandLine
{
public:
	WolfwizardCommandLine( int argc, char **argv, const std::string& referencePath_, const std::string& modulePath);
	~WolfwizardCommandLine(){}

	bool printhelp() const						{return m_printhelp;}
	bool printversion() const					{return m_printversion;}
	const std::string& configfile() const				{return m_configfile;}

	static void print( std::ostream& out);
	const config::ConfigurationNode& providerconfig()		 const	{return m_providerconfig;}
	const std::list<std::string>& modules() const			{return m_modules;}
	const std::string& referencePath() const			{return m_referencePath;}

private:
	bool m_printhelp;
	bool m_printversion;
	std::string m_configfile;
	config::ConfigurationTree m_config;
	config::ConfigurationNode m_providerconfig;
	std::string m_referencePath;
	std::string m_modulePath;
	std::list<std::string> m_modules;
};

}}//namespace
#endif

