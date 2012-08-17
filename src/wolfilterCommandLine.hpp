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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file wolfilterCommandLine.hpp
///\brief Interface for the options of a wolfilter call
#ifndef _Wolframe_WOLFILTER_OPTIONS_HPP_INCLUDED
#define _Wolframe_WOLFILTER_OPTIONS_HPP_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace config {

class WolfilterCommandLine
{
public:
	WolfilterCommandLine(int argc, char **argv);
	WolfilterCommandLine( const WolfilterCommandLine& o)
		:m_printhelp(o.m_printhelp)
		,m_printversion(o.m_printversion)
		,m_inputfile(o.m_inputfile)
		,m_scripts(o.m_scripts)
		,m_modules(o.m_modules)
		,m_forms(o.m_forms)
		,m_peerformfunctions(o.m_peerformfunctions)
		,m_cmd(o.m_cmd)
		,m_inputfilter(o.m_inputfilter)
		,m_outputfilter(o.m_outputfilter)
		,m_helpstring(o.m_helpstring)
		,m_inbufsize(o.m_inbufsize)
		,m_outbufsize(o.m_outbufsize)
		{}
	~WolfilterCommandLine(){}

	///\class FormParam
	///\brief Definition of a form on command line
	struct FormParam
	{
		std::string ddlname;
		std::string filename;
	};

	///\class PeerFormFunctionParam
	///\brief Definition of a peer form function on command line
	struct PeerFormFunctionParam
	{
		std::string name;
		std::string functionname;
		std::string inputformname;
		std::string outputformname;
	};

	bool printhelp() const								{return m_printhelp;}
	bool printversion() const							{return m_printversion;}
	const std::string& inputfile() const						{return m_inputfile;}
	const std::vector<std::string>& luaimports() const				{return m_luaimports;}
	const std::vector<std::string>& scripts() const					{return m_scripts;}
	const std::vector<std::string>& modules() const					{return m_modules;}
	const std::vector<FormParam>& forms() const					{return m_forms;}
	const std::vector<PeerFormFunctionParam>& peerformfunctions() const		{return m_peerformfunctions;}
	const std::string& cmd() const							{return m_cmd;}
	const std::string& inputfilter() const						{return m_inputfilter;}
	const std::string& outputfilter() const						{return m_outputfilter;}

	void print(std::ostream &) const;

	std::size_t inbufsize() const							{return m_inbufsize;}
	std::size_t outbufsize() const							{return m_outbufsize;}

	///\brief loads the command line objects into the global context
	void loadGlobalContext( const std::string& referencePath, module::ModulesDirectory& modDir) const;

private:
	bool m_printhelp;
	bool m_printversion;
	std::string m_inputfile;
	std::vector<std::string> m_luaimports;
	std::vector<std::string> m_scripts;
	std::vector<std::string> m_modules;
	std::vector<FormParam> m_forms;
	std::vector<PeerFormFunctionParam> m_peerformfunctions;
	std::string m_cmd;
	std::string m_inputfilter;
	std::string m_outputfilter;
	std::string m_helpstring;
	std::size_t m_inbufsize;
	std::size_t m_outbufsize;
};

}}//namespace
#endif

