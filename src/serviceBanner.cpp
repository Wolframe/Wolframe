//
//
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "logger.hpp"

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
	namespace Configuration	{

/// Service signature

bool ServiceBanner::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
	if ( boost::algorithm::iequals( node, "SignatureTokens" ))	{
//		if ( !getStringValue( pt, displayName(), "SignatureTokens", tokens ))	return false;
	}
	else	{
		LOG_WARNING << displayName() << ": unknown configuration option: <" << node << ">";
		return false;
	}
	return true;
}


bool ServiceBanner::check() const
{
	switch ( tokens )	{
	case PRODUCT_NAME:
	case VERSION_MAJOR:
	case VERSION_MINOR:
	case VERSION_REVISION:
	case PRODUCT_OS:
	case NONE:
	case UNDEFINED:
		return true;
	default:
		LOG_ERROR << "Unknown value for ServiceBanner::tokens: " << (int)tokens;
		return false;
	}
	// for stupid compilers
}


void ServiceBanner::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	os << "   Service banner: ";
	switch ( tokens )	{
	case PRODUCT_NAME:	os << "product name only"; break;
	case VERSION_MAJOR:	os << "product name and major version"; break;
	case VERSION_MINOR:	os << "product name and minor version"; break;
	case VERSION_REVISION:	os << "product name and revision"; break;
	case PRODUCT_OS:	os << "product name, revision and OS"; break;
	case NONE:		os << "none"; break;
	case UNDEFINED:
	default:		os << " NOT DEFINED !"; break;
	}
	os << std::endl;
	os << "   Print service name: " << ( serverName ? "yes" : "no" ) << std::endl;
}


const std::string ServiceBanner::toString() const
{
	std::string ret;
	ret = "bla bla";
	return ret;
}

	} // namespace Configuration
} // namespace _Wolframe
