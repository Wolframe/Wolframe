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
#include "langbind/appObjects.hpp"
#include "database/DBprovider.hpp"
#include "langbind/appConfig_struct.hpp"
#include "langbind/directmapConfig_option.hpp"
#include "filter/ptreefilter.hpp"
#include "filter/tostringfilter.hpp"
#include "moduleInterface.hpp"
#include "config/ConfigurationTree.hpp"
#include "serialize/structOptionParser.hpp"
#include "utils/miscUtils.hpp"
#include "utils/doctype.hpp"
#include "config/structSerialize.hpp"
#include "logger-v1.hpp"
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <cstring>
#include <algorithm>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

using namespace _Wolframe;
using namespace _Wolframe::config;

config::ConfigurationTree WolfilterCommandLine::getDBProviderConfigTree() const
{
	boost::property_tree::ptree rt;
	if (!m_dbconfig.empty())
	{
		std::vector<std::pair<std::string,std::string> >
			dbhl = m_modulesDirectory.getConfigurableSectionKeywords( ObjectConstructorBase::DATABASE_OBJECT);
		if (dbhl.size() > 1)
		{
			// only one database module allowed:
			throw std::runtime_error( "more than one database module loaded for wolfilter");
		}
		rt.add_child( dbhl.begin()->second, m_dbconfig);
	}
	return rt;
}

config::ConfigurationTree WolfilterCommandLine::getProcProviderConfigTree() const
{
	boost::property_tree::ptree proccfg;
	try
	{
		boost::property_tree::ptree envcfg = m_envconfig.toPropertyTree();
		if (!m_dbconfig.empty())
		{
			std::string dbLabel = m_dbconfig.get<std::string>( "identifier");
			if (dbLabel.empty())
			{
				throw std::runtime_error( "database configuration without 'identifier' field");
			}
			proccfg.add_child( "database", boost::property_tree::ptree( dbLabel));
		}
		if (!envcfg.empty())
		{
			proccfg.add_child( "environment", envcfg);
		}
		std::vector<std::pair<std::string,std::string> >
			cmdhl = m_modulesDirectory.getConfigurableSectionKeywords( ObjectConstructorBase::CMD_HANDLER_OBJECT);

		if (!m_scriptenvconfig.script.empty())
		{
			std::vector<langbind::ScriptCommandConfigStruct>::const_iterator ci = m_scriptenvconfig.script.begin(), ce = m_scriptenvconfig.script.end();
			std::string extension = utils::getFileExtension( ci->file);
			if (extension.empty())
			{
				throw std::runtime_error( "script without extension specified. Cannot assign it to one command handler");
			}
			for (++ci; ci!=ce; ++ci)
			{
				if (!boost::iequals( extension, utils::getFileExtension(ci->file)))
				{
					throw std::runtime_error( "multiple scripts with different extension specified. Cannot assign them to one command handler");
				}
			}
			std::string cmdhndname = std::string( extension.c_str() +1);

			std::pair<std::string,std::string> cfgid;
			bool cfgid_set = false;
			std::vector<std::pair<std::string,std::string> >::const_iterator pi = cmdhl.begin(), pe = cmdhl.end();
			for (; pi != pe; ++pi)
			{
				if (boost::istarts_with( pi->second, cmdhndname))
				{
					if (cfgid_set)
					{
						throw std::runtime_error( std::string( "more than one command handler module loaded that match to scripts selected (") + cmdhndname + ")");
					}
					cfgid = *pi;
					cfgid_set = true;
				}
			}
			if (!cfgid_set)
			{
				throw std::runtime_error( std::string( "no command handler module loaded that matches to scripts selected (") + cmdhndname + ")");
			}
			boost::property_tree::ptree cmdhlcfg;
			cmdhlcfg.add_child( cfgid.second, m_scriptenvconfig.toPropertyTree());
			proccfg.add_child( cfgid.first, cmdhlcfg);
		}
		if (!m_directmapconfig.command.empty())
		{
			std::pair<std::string,std::string> cfgid;
			bool cfgid_set = false;
			std::vector<std::pair<std::string,std::string> >::const_iterator pi = cmdhl.begin(), pe = cmdhl.end();
			for (; pi != pe; ++pi)
			{
				if (boost::istarts_with( pi->second, "directmap"))
				{
					if (cfgid_set)
					{
						throw std::runtime_error( "more than one command handler module loaded that matches to directmap");
					}
					cfgid = *pi;
					cfgid_set = true;
				}
			}
			if (!cfgid_set)
			{
				throw std::runtime_error( "no command handler module loaded that matches to directmap");
			}
			boost::property_tree::ptree cmdhlcfg;
			cmdhlcfg.add_child( cfgid.second, m_directmapconfig.toPropertyTree());
			proccfg.add_child( cfgid.first, cmdhlcfg);
		}
		std::vector<std::string>::const_iterator pi = m_programs.begin(), pe = m_programs.end();
		for (; pi != pe; ++pi)
		{
			proccfg.add_child( "programFile", boost::property_tree::ptree( *pi));
		}
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string( "could not build wolframe configuration from given options: ") + e.what());
	}
	return proccfg;
}

static std::string configurationTree_tostring( const boost::property_tree::ptree& pt)
{
	langbind::TypedInputFilterR inp( new langbind::PropertyTreeInputFilter( pt));
	langbind::ToStringFilter* res;
	langbind::TypedOutputFilterR out( res = new langbind::ToStringFilter( "\t"));
	langbind::RedirectFilterClosure redirect( inp, out);
	if (!redirect.call()) throw std::runtime_error( "can't map configuration tree to string");
	return res->content();
}

namespace po = boost::program_options;

struct OptionStruct
{
	po::options_description fopt;
	po::positional_options_description popt;

	OptionStruct()
		:fopt("Options")
	{
		fopt.add_options()
			( "version,v", "print version" )
			( "help,h", "print help message" )
			( "loglevel,l", po::value<std::string>(), "specify the log level on console" )
			( "input,f", po::value<std::string>(), "specify input file to process by path" )
			( "input-filter,i", po::value<std::string>(), "specify input filter by name" )
			( "output-filter,o", po::value<std::string>(), "specify output filter by name" )
			( "module,m", po::value< std::vector<std::string> >(), "specify module to load by path" )
			( "program,p", po::value< std::vector<std::string> >(), "specify program to load by path" )
			( "script,s", po::value< std::vector<std::string> >(), "specify script to load by path" )
			( "directmap,d", po::value< std::vector<std::string> >(), "specify directmap definition" )
			( "form,F", po::value< std::vector<std::string> >(), "specify form to load by path" )
			( "printlayout,P", po::value< std::vector<std::string> >(), "specify print layout for a form" )
			( "database,D", po::value<std::string>(), "specifiy transaction database" )
			( "normalize,N", po::value< std::vector<std::string> >(), "specify normalization function" )
			( "cmd", po::value<std::string>(), "name of the command to execute")
			;

		popt.add( "cmd", 1);
	}
};

WolfilterCommandLine::WolfilterCommandLine( int argc, char** argv, const std::string& referencePath)
	:m_printhelp(false)
	,m_printversion(false)
	,m_inbufsize(8<<10)
	,m_outbufsize(8<<10)
	,m_referencePath(referencePath)
{
	static const OptionStruct ost;
	po::variables_map vmap;
	try
	{
		po::store( po::command_line_parser(argc, argv).options(ost.fopt).positional(ost.popt).run(), vmap);
		po::notify( vmap);
	}
	catch (std::exception& e)
	{
		throw std::runtime_error( std::string("error parsing command line options: ") + e.what());
	}
	m_printversion = vmap.count( "version");
	m_printhelp = vmap.count( "help");
	if (vmap.count( "loglevel"))
	{
		m_loglevel = vmap["loglevel"].as<std::string>();
		_Wolframe::log::LogBackend::instance().setConsoleLevel( log::LogLevel::strToLogLevel( m_loglevel));
	}
	if (vmap.count( "input")) m_inputfile = vmap["input"].as<std::string>();
	if (vmap.count( "module"))
	{
		m_modules = vmap["module"].as<std::vector<std::string> >();
		std::vector<std::string>::iterator itr=m_modules.begin(), end=m_modules.end();
		for (; itr != end; ++itr)
		{
			*itr = utils::getCanonicalPath( *itr, referencePath);
		}
	}
	if (vmap.count( "program"))
	{
		m_programs = vmap["program"].as<std::vector<std::string> >();
	}
	if (vmap.count( "form"))
	{
		std::vector<std::string> formparams = vmap["form"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=formparams.begin(), end=formparams.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.form.push_back( langbind::DDLFormOption( *itr));
		}
	}
	if (vmap.count( "printlayout"))
	{
		std::vector<std::string> printlayouts = vmap["printlayout"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=printlayouts.begin(), end=printlayouts.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.printlayout.push_back( langbind::PrintLayoutOption( *itr));
		}
	}
	if (vmap.count( "normalize"))
	{
		std::vector<std::string> normalizers = vmap["normalize"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=normalizers.begin(), end=normalizers.end();
		for (; itr != end; ++itr)
		{
			m_envconfig.normalize.push_back( langbind::NormalizeFunctionOption( *itr));
		}
	}
	if (vmap.count( "script"))
	{
		std::vector<std::string> scripts = vmap["script"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=scripts.begin(), end=scripts.end();
		for (; itr != end; ++itr)
		{
			m_scriptenvconfig.script.push_back( langbind::ScriptCommandOption( *itr));
		}
	}
	if (vmap.count( "directmap"))
	{
		std::vector<std::string> directmaps = vmap["directmap"].as<std::vector<std::string> >();
		std::vector<std::string>::const_iterator itr=directmaps.begin(), end=directmaps.end();
		for (; itr != end; ++itr)
		{
			m_directmapconfig.command.push_back( langbind::DirectmapCommandOption( *itr));
		}
	}

	if (vmap.count( "cmd")) m_cmd = vmap["cmd"].as<std::string>();
	if (vmap.count( "input-filter")) m_inputfilter = vmap["input-filter"].as<std::string>();
	if (vmap.count( "output-filter")) m_outputfilter = vmap["output-filter"].as<std::string>();
	if (vmap.count( "database")) m_dbconfig = serialize::structOptionTree( vmap["database"].as<std::string>());

	if (m_outputfilter.empty() && !m_inputfilter.empty()) m_outputfilter = m_inputfilter; //... default same filter for input and output
	if (m_inputfilter.empty() && !m_outputfilter.empty()) m_inputfilter = m_outputfilter; //... default same filter for input and output

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
	dd << ost.fopt;
	m_helpstring = dd.str();

	// Load modules
	std::list<std::string> modfiles;
	std::copy( m_modules.begin(), m_modules.end(), std::back_inserter(modfiles));
	if (!LoadModules( m_modulesDirectory, modfiles))
	{
		throw std::runtime_error( "Modules could not be loaded");
	}

	// Load, instantiate and check the configuration:
	m_dbProviderConfig.reset( new db::DBproviderConfig());
	config::ConfigurationTree dbcfg = getDBProviderConfigTree();

	LOG_DEBUG << "Created database provider configuration from command line:";
	LOG_DEBUG << configurationTree_tostring( dbcfg);

	if (!m_dbProviderConfig->parse( dbcfg, "", &m_modulesDirectory))
	{
		throw std::runtime_error( "Database provider configuration could not be created from command line");
	}
	m_dbProviderConfig->setCanonicalPathes( referencePath);
	if (!m_dbProviderConfig->check())
	{
		throw std::runtime_error( "error in command line. failed to setup a valid database provider configuration");
	}
	m_procProviderConfig.reset( new proc::ProcProviderConfig());
	config::ConfigurationTree ppcfg = getProcProviderConfigTree();

	LOG_DEBUG << "Created processor provider configuration from command line:";
	LOG_DEBUG << configurationTree_tostring( ppcfg);

	if (!m_procProviderConfig->parse( ppcfg, "", &m_modulesDirectory))
	{
		throw std::runtime_error( "Processor provider configuration could not be created from command line");
	}
	m_procProviderConfig->setCanonicalPathes( referencePath);
	if (!m_procProviderConfig->check())
	{
		throw std::runtime_error( "error in command line. failed to setup a valid processor provider configuration");
	}
}

void WolfilterCommandLine::print(std::ostream& out) const
{
	out << "Call:" << std::endl;
	out << "\twolfilter [OPTION] <cmd> <inputfilter> <outputfilter>" << std::endl;
	out << m_helpstring << std::endl;
}



