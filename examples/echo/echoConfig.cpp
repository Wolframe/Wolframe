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

} // namespace config


void EchoConfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Idle timeout: " << timeout << std::endl;
}


/// Check if the database configuration makes sense
bool EchoConfiguration::check() const
{
	return true;
}


bool EchoConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "idle" ))	{
			if ( !config::Parser::getValue( logPrefix().c_str(), *L1it, timeout ))
				retVal = false;
		}
		else	{
			LOG_WARNING << logPrefix() << ": unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
//	if ( timeout == 0 )
//		timeout = DEFAULT_TIMEOUT;

	return retVal;
}

} // namespace _Wolframe
