//
// echo configuration functions
//

#include "handlerConfig.hpp"
#include "configHelpers.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>


static const unsigned short DEFAULT_TIMEOUT = 180;


namespace _Wolframe	{

void EchoConfiguration::print( std::ostream& os ) const
{
	os << displayStr() << std::endl;
	os << "   Timeout: " << timeout << std::endl;
}


/// Check if the database configuration makes sense
bool EchoConfiguration::check( std::ostream& ) const
{

	return true;
}


bool EchoConfiguration::parse( const boost::property_tree::ptree& pt, std::ostream& os )
{
	for ( boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++ )	{
		if ( boost::algorithm::iequals( it->first, "idle" ))	{
			if ( !Configuration::getUnsignedShortValue( it, displayStr(), "idle", timeout, os ))
				return false;
		}
		else	{
			os << displayStr() << ": unknown configuration option: <" << it->first << ">";
			return false;
		}
	}
//	if ( timeout == 0 )
//		timeout = DEFAULT_TIMEOUT;

	return true;
}

} // namespace _Wolframe
