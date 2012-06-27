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
///\file wolfilterCommandLine.cpp
///\brief Implementation of the options of a wolfilter call
#include "wolfilterCommandLine.hpp"
#include "langbind/appGlobalContext.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

using namespace _Wolframe;
using namespace config;

WolfilterCommandLine::WolfilterCommandLine( int argc, char** argv)
	:m_printhelp(false)
	,m_printversion(false)
	,m_inbufsize(8<<10)
	,m_outbufsize(8<<10)
{
	namespace po = boost::program_options;

	po::options_description fopt("Options");
	fopt.add_options()
		( "version,v", "print version" )
		( "help,h", "print help message" )
		( "input,f", po::value<std::string>(), "specify input file to process by path" )
		( "module,m", po::value< std::vector<std::string> >(), "specify module to load by path" )
		( "form,r", po::value< std::vector<std::string> >(), "specify form to load by path" )
		( "script,s", po::value< std::vector<std::string> >(), "specify script to load by path" )
		( "cmd", po::value<std::string>(), "name of the command to execute")
		( "input-filter,i", po::value<std::string>(), "specify input filter by name" )
		( "output-filter,o", po::value<std::string>(), "specify output filter by name" )
		;

	po::positional_options_description popt;
	popt.add( "cmd", 1);
	popt.add( "output-filter", 1);
	popt.add( "input-filter", 1);

	po::variables_map vmap;
	po::store( po::command_line_parser(argc, argv).options(fopt).positional(popt).run(), vmap);
	po::notify( vmap);

	m_printversion = vmap.count( "version");
	m_printhelp = vmap.count( "help");

	if (vmap.count( "input")) m_inputfile = vmap["input"].as<std::string>();
	if (vmap.count( "module")) m_modules = vmap["module"].as<std::vector<std::string> >();
	if (vmap.count( "form"))
	{
		std::vector<std::string> formparams = vmap["form"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=formparams.begin(), end=formparams.end();
		for (; itr != end; ++itr)
		{
			FormParam formparam;
			const char* cc = std::strchr( itr->c_str(), '#');
			if (cc)
			{
				formparam.ddlname = std::string( itr->c_str(), cc-itr->c_str());
				formparam.filename = std::string( cc+1);
			}
			else
			{
				std::string ext = utils::getFileExtension( *itr);
				if (!ext.size()) throw std::runtime_error( "no DDL specified (file extension missing) for form file");

				formparam.ddlname = std::string( ext.c_str()+1);
				formparam.filename = *itr;
			}
			formparam.formname = utils::getFileStem( formparam.filename);
			m_forms.push_back( formparam);
		}
	}
	if (vmap.count( "script")) m_scripts = vmap["script"].as<std::vector<std::string> >();
	if (vmap.count( "cmd")) m_cmd = vmap["cmd"].as<std::string>();
	if (vmap.count( "input-filter")) m_inputfilter = vmap["input-filter"].as<std::string>();
	if (vmap.count( "output-filter")) m_outputfilter = vmap["output-filter"].as<std::string>();

	if (m_outputfilter.empty()) m_outputfilter = m_inputfilter;		//... default same filter for input and output
	if (m_inputfilter.empty()) m_inputfilter = "xml:textwolf";		//... xml:textwolf as default input filter
	if (m_outputfilter.empty()) m_outputfilter = m_inputfilter;		//... default same filter for input and output

	const char* bp;
	bp = std::strchr( m_inputfilter.c_str(), '/');
	if (bp)
	{
		m_inbufsize = (std::size_t)atoi( bp+1);
		m_inputfilter.resize( bp-m_inputfilter.c_str());
	}
	bp = std::strchr( m_outputfilter.c_str(), '/');
	if (bp)
	{
		m_outbufsize = (std::size_t)atoi( bp+1);
		m_outputfilter.resize( bp-m_outputfilter.c_str());
	}

	std::ostringstream dd;
	dd << fopt;
	m_helpstring = dd.str();
}

void WolfilterCommandLine::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd> <inputfilter> <outputfilter>" << std::endl;
	out << m_helpstring << std::endl;
}

static std::string canonicalPath( const std::string& path, const std::string& refpath)
{
	boost::filesystem::path pt( path);
	if (pt.is_absolute())
	{
		return pt.string();
	}
	else
	{
		return boost::filesystem::absolute( pt, boost::filesystem::path( refpath)).string();
	}
}

void WolfilterCommandLine::loadGlobalContext( const std::string& referencePath) const
{
	langbind::GlobalContext* gct = langbind::getGlobalContext();
	{
		std::vector<std::string>::const_iterator itr,end;
		itr = scripts().begin();
		end = scripts().end();
		for (; itr != end; ++itr)
		{
			std::string scriptpath( canonicalPath( *itr, referencePath));
			langbind::LuaScript script( scriptpath);
			std::vector<std::string>::const_iterator fi = script.functions().begin(), fe = script.functions().end();
			for (; fi != fe; ++fi)
			{
				gct->defineLuaFunction( *fi, script);
			}
		}
	}
	{
		std::vector<WolfilterCommandLine::FormParam>::const_iterator itr,end;
		itr = forms().begin();
		end = forms().end();
		for (; itr != end; ++itr)
		{
			std::string formpath( canonicalPath( itr->filename, referencePath));
			ddl::CompilerInterfaceR ci;
			if (!gct->getDDLCompiler( itr->ddlname, ci))
			{
				throw std::runtime_error( "Unknown DDL in form parameter");
			}
			else
			{
				ddl::StructTypeR form = ddl::StructTypeR( new ddl::StructType());
				std::string error;
				*form = ci->compileFile( formpath);
				gct->defineForm( itr->formname, form);
			}
		}
	}
}





