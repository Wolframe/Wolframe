#include "luaCommandHandlerBuilder.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::module;

bool LuaCommandHandlerConfig::parse( const config::ConfigurationNode& pt, const std::string&, const module::ModulesDirectory* modules)
{
	m_modules = modules;
	config::ConfigurationNode::const_iterator pi = pt.begin(), pe = pt.end();
	try
	{
		for (; pi != pe; ++pi)
		{
			if (boost::iequals( pi->first, "filter"))
			{
				std::vector<std::string> filterdef;
				utils::splitString( filterdef, pi->second.data().string(), "=");
				if (filterdef.size() == 1)
				{
					m_context.setDefaultFilter( "", filterdef.at(0));
				}
				else if (filterdef.size() == 2)
				{
					m_context.setDefaultFilter( filterdef.at(0), filterdef.at(1));
				}
				else
				{
					throw std::runtime_error( "illegal value for filter declaration. expected two items separated by a '='");
				}
			}
			else if (boost::iequals( pi->first, "program"))
			{
				m_programfiles.push_back( pi->second.data());
			}
			else
			{
				throw std::runtime_error( std::string("expected 'program' or 'filter' definition instead of '") + pi->first + "'");
			}
		}
	}
	catch (std::runtime_error& e)
	{
		LOG_ERROR << "lua command handler configuration error " << pi->position.logtext() << ":" << e.what();
		return false;
	}
	return true;
}

void LuaCommandHandlerConfig::setCanonicalPathes( const std::string& referencePath)
{
	std::vector<std::string>::iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		*pi = utils::getCanonicalPath( *pi, referencePath);
	}
	m_context.loadPrograms( m_programfiles);
}

bool LuaCommandHandlerConfig::check() const
{
	bool rt = true;
	std::vector<std::string>::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		if (!utils::fileExists( *pi))
		{
			LOG_ERROR << "program file '" << *pi << "' does not exist";
			rt = false;
		}
	}
	return rt;
}

void LuaCommandHandlerConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indentstr( indent*3, ' ');
	std::vector<std::string>::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		os << indentstr << "program " << *pi;
	}
}

