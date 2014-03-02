#include "directmapCommandHandlerBuilder.hpp"
#include "logger-v1.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::module;

bool DirectmapCommandHandlerConfig::parse( const config::ConfigurationTree& pt, const std::string&, const module::ModulesDirectory*)
{
	int filterDefied = 0;
	try
	{
		boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();
		for (; pi != pe; ++pi)
		{
			// optional configuration parameters:
			if (boost::iequals( pi->first, "filter"))
			{
				++filterDefied;

				std::vector<std::string> filterdef;
				utils::splitString( filterdef, pi->second.get_value<std::string>(), "=");
				if (filterdef.size() == 1)
				{
					m_context.setFilter( "", filterdef.at(0));
				}
				else if (filterdef.size() == 2)
				{
					m_context.setFilter( filterdef.at(0), filterdef.at(1));
				}
				else
				{
					throw std::runtime_error( "illegal value for filter declaration. expected two items separated by a '='");
				}
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
		if (!filterDefied)
		{
			LOG_WARNING << "no filter defined in directmap command handler. cannot process anything";
		}
	}
	catch (std::runtime_error& e)
	{
		LOG_ERROR << e.what();
		return false;
	}
	return true;
}

void DirectmapCommandHandlerConfig::setCanonicalPathes( const std::string& referencePath)
{
	std::vector<std::string>::iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		*pi = utils::getCanonicalPath( *pi, referencePath);
	}
	m_context.loadPrograms( m_programfiles);
}

bool DirectmapCommandHandlerConfig::check() const
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

bool DirectmapCommandHandlerConfig::checkReferences( const proc::ProcessorProviderInterface* provider) const
{
	return m_context.checkReferences( provider);
}

void DirectmapCommandHandlerConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indentstr( indent*3, ' ');
	std::vector<std::string>::const_iterator pi = m_programfiles.begin(), pe = m_programfiles.end();
	for (; pi != pe; ++pi)
	{
		os << indentstr << "program " << *pi;
	}
}

