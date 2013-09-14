#include "luaCommandHandlerBuilder.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::module;

bool LuaCommandHandlerConfig::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory* modules)
{
	int filterDefied = 0;
	m_modules = modules;
	try
	{
		boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
		for (; pi != pe; ++pi)
		{
			// optional configuration parameters:
			if (boost::iequals( pi->first, "filter"))
			{
				if (filterDefied++) throw std::runtime_error( "duplicate default filter definition in configuration");
				std::string filtername = pi->second.get_value<std::string>();
				if (filtername.empty()) throw std::runtime_error( "expected non empty default filter definition");
				m_context.setDefaultFilter( filtername);
			}
			// required configuration parameters:
			else if (boost::iequals( pi->first, "program"))
			{
				m_programfiles.push_back( pi->second.get_value<std::string>());
			}
			else
			{
				throw std::runtime_error( std::string("expected 'program' or 'filter' definition instead of '") + pi->first + "'");
			}
		}
	}
	catch (std::runtime_error& e)
	{
		LOG_ERROR << e.what();
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
	m_context.loadPrograms( m_programfiles, m_modules);
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

