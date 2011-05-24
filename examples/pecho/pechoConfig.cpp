//
// echo configuration functions
//

#include "handlerConfig.hpp"
#include "config/valueParser.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>

static const unsigned short DEFAULT_TIMEOUT = 180;

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( pEchoConfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	bool retVal = true;
	bool isSet = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "idle" ))	{
			if ( !config::Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.timeout ))
				retVal = false;
			isSet = true;
		}
		else
			LOG_WARNING << cfg.logPrefix() << ": unknown configuration option: '"
				    << L1it->first << "'";
	}
	if ( !isSet )
		cfg.timeout = DEFAULT_TIMEOUT;

	return retVal;
}

} // namespace config

void pEchoConfiguration::print( std::ostream& os, size_t /*indent*/ ) const
{
	os << sectionName() << std::endl;
	os << "   Idle timeout: " << timeout << std::endl;
}


/// Check if the database configuration makes sense
bool pEchoConfiguration::check() const
{
	return true;
}

} // namespace _Wolframe
