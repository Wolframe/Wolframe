//
// echo configuration functions
//

#include "handlerConfig.hpp"
#include "configHelpers.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>


static const unsigned short DEFAULT_TIMEOUT = 180;


namespace _Wolframe	{

void EchoConfiguration::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	os << "   Idle timeout: " << timeout << std::endl;
}


/// Check if the database configuration makes sense
bool EchoConfiguration::check() const
{

	return true;
}


bool EchoConfiguration::parse( const boost::property_tree::ptree::const_iterator it,
			       const std::string& /* nodeName */ )
{
	for ( boost::property_tree::ptree::const_iterator L1it = it->second.begin();
								L1it != it->second.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "idle" ))	{
			if ( !Configuration::getUnsignedShortValue( L1it, displayName(), timeout ))
				return false;
		}
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <"
				    << L1it->first << ">";
//			return false;
		}
	}
//	if ( timeout == 0 )
//		timeout = DEFAULT_TIMEOUT;

	return true;
}

} // namespace _Wolframe
