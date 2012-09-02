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
///\file wolfilterCommandLine.cpp
///\brief Implementation of the options of a wolfilter call
#include "wolfilterCommandLine.hpp"
#include "langbind/appGlobalContext.hpp"
#include "langbind/appConfig_struct.hpp"
#include "moduleInterface.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/miscUtils.hpp"
#include "utils/doctype.hpp"

using namespace _Wolframe;
using namespace _Wolframe::config;

static void parseNameTypeSource( const std::string& opt, bool getFilenameStemAsName, std::string& name, std::string& type, std::string& sourcepath)
{
	const char* cc = std::strchr( opt.c_str(), ':');
	const char* xx = std::strchr( opt.c_str(), '.');
	if (cc && cc-opt.c_str() > 1)
	{
		if (xx && xx < cc)
		{
			std::string type_name( opt.c_str(), cc-opt.c_str());
			std::string ext = utils::getFileExtension( type_name);
			std::string stem = utils::getFileStem( type_name);

			if (!ext.empty()) type = std::string( ext.c_str() + 1);
			if (!stem.empty()) name = std::string( stem.c_str());
		}
		else
		{
			type = std::string( opt.c_str(), cc-opt.c_str());
			name = "";
		}
		sourcepath = std::string( cc+1);
		if (type.empty())
		{
			std::string ext = utils::getFileExtension( sourcepath);
			if (!ext.empty()) type = std::string( ext.c_str() + 1);
		}
		if (name.empty() && getFilenameStemAsName)
		{
			std::string stem = utils::getFileStem( sourcepath);
			if (!stem.empty()) name = std::string( stem.c_str());
		}
	}
	else
	{
		sourcepath = opt;
		std::string ext = utils::getFileExtension( opt);
		if (ext.empty()) throw std::runtime_error( "no type of print layout specified (file extension missing)");
		type = std::string( ext.c_str() + 1);

		if (getFilenameStemAsName)
		{
			std::string stem = utils::getFileStem( opt);
			if (stem.empty()) throw std::runtime_error( "no name of print layout specified (file stem missing)");
			name = std::string( stem.c_str());
		}
	}
}

struct DDLFormOption :public langbind::DDLFormConfigStruct
{
	DDLFormOption( const std::string& src)
	{
		const char* cc = std::strchr( src.c_str(), ':');
		if (cc && cc-src.c_str() > 1)
		{
			DDL = std::string( src.c_str(), cc-src.c_str());
			sourcepath = std::string( cc+1);
		}
		else
		{
			std::string ext = utils::getFileExtension( src);
			if (!ext.size()) throw std::runtime_error( "no DDL specified (file extension missing) for form file");

			DDL = std::string( ext.c_str()+1);
			sourcepath = src;
		}
	}
};

struct PrintLayoutOption :public langbind::PrintLayoutConfigStruct
{
	PrintLayoutOption( const std::string& opt)
	{
		parseNameTypeSource( opt, true, name, type, sourcepath);
	}
};

struct TransactionFunctionOption :public langbind::TransactionFunctionConfigStruct
{
	TransactionFunctionOption( const std::string& src)
	{
		std::string::const_iterator si = src.begin(), se = src.end();
		if (!utils::parseNextToken( name, si, se))
		{
			throw std::runtime_error( "illegal transaction function option (name)");
		}
		if (!utils::parseNextToken( type, si, se))
		{
			throw std::runtime_error( "illegal transaction function option (type)");
		}
		if (!utils::parseNextToken( database, si, se))
		{
			throw std::runtime_error( "illegal transaction function option (database)");
		}
		call.insert( call.end(), si, se);
	}
};

struct ScriptCommandOption :public langbind::ScriptCommandConfigStruct
{
	ScriptCommandOption( const std::string& opt)
	{
		parseNameTypeSource( opt, false, name, type, sourcepath);
	}
};


WolfilterCommandLine::WolfilterCommandLine( int argc, char** argv, const std::string& referencePath)
	:m_printhelp(false)
	,m_printversion(false)
	,m_inbufsize(8<<10)
	,m_outbufsize(8<<10)
	,m_referencePath(referencePath)
{
	namespace po = boost::program_options;

	po::options_description fopt("Options");
	fopt.add_options()
		( "version,v", "print version" )
		( "help,h", "print help message" )
		( "input,f", po::value<std::string>(), "specify input file to process by path" )
		( "module,m", po::value< std::vector<std::string> >(), "specify module to load by path" )
		( "form,r", po::value< std::vector<std::string> >(), "specify form to load by path" )
		( "printlayout,p", po::value< std::vector<std::string> >(), "specify print layout for a form" )
		( "transaction,t", po::value< std::vector<std::string> >(), "specify transaction function" )
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
			m_envconfig.form.push_back( DDLFormOption( *itr));
		}
	}
	if (vmap.count( "printlayout"))
	{
		std::vector<std::string> printlayouts = vmap["printlayout"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=printlayouts.begin(), end=printlayouts.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.printlayout.push_back( PrintLayoutOption( *itr));
		}
	}
	if (vmap.count( "transaction"))
	{
		std::vector<std::string> transactions = vmap["transaction"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=transactions.begin(), end=transactions.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.transaction.push_back( TransactionFunctionOption( *itr));
		}
	}
	if (vmap.count( "script"))
	{
		std::vector<std::string> scripts = vmap["script"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=scripts.begin(), end=scripts.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.script.push_back( ScriptCommandOption( *itr));
		}
	}

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

	// Load modules
	std::list<std::string> modfiles;
	std::vector<std::string>::const_iterator itr,end;
	itr = m_modules.begin();
	end = m_modules.end();

	for (; itr != end; ++itr)
	{
		modfiles.push_back( utils::getCanonicalPath( *itr, referencePath));
	}
	if (!LoadModules( m_modulesDirectory, modfiles))
	{
		throw std::runtime_error( "Modules could not be loaded");
	}

	// Set canonical pathes
	m_envconfig.setCanonicalPathes( referencePath);

	// Define processor provider objects
}

void WolfilterCommandLine::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd> <inputfilter> <outputfilter>" << std::endl;
	out << m_helpstring << std::endl;
}

void WolfilterCommandLine::loadGlobalContext() const
{
	langbind::GlobalContext* gct = langbind::getGlobalContext();
	{
		if (!gct) throw std::runtime_error( "No global context defined");
		gct->load( m_envconfig);
	}
}


