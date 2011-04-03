//
// serviceBanner.cpp
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "logger.hpp"

#include <string>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
	namespace Configuration	{


static ServiceBanner::SignatureTokens strToToken( std::string& str )
{
	if ( boost::algorithm::iequals( str, "ProductOnly" ))
		return ServiceBanner::PRODUCT_NAME;
	else if ( boost::algorithm::iequals( str, "Major" ))
		return ServiceBanner::VERSION_MAJOR;
	else if ( boost::algorithm::iequals( str, "Minor" ))
		return ServiceBanner::VERSION_MINOR;
	else if ( boost::algorithm::iequals( str, "Revision" ))
		return ServiceBanner::VERSION_REVISION;
	else if ( boost::algorithm::iequals( str, "OS" ))
		return ServiceBanner::PRODUCT_OS;
	else if ( boost::algorithm::iequals( str, "None" ))
		return ServiceBanner::NONE;
	return ServiceBanner::UNDEFINED;
}

/// Service signature
bool ServiceBanner::parse( const boost::property_tree::ptree::const_iterator it,
			   const std::string& node )
{
	if ( boost::algorithm::iequals( node, "ServerTokens" ))	{
		std::string val;
		if ( !getStringValue( it, displayName(), val ))
			return false;
		else	{
			if (( tokens_ = strToToken( val )) == ServiceBanner::UNDEFINED )	{
				LOG_ERROR << displayName() << ": unknown ServerTokens option: \""
					  << val << "\"";
				return false;
			}
		}
	}
	else if ( boost::algorithm::iequals( node, "ServerSignature" ))	{
		if ( !getBoolValue( it, displayName(), serverName_, serverNameDefined_ ))
			return false;
	}
	else	{
		LOG_FATAL << displayName() << ": called with unknown configuration option: \""
			  << node << "\"";
		return false;
	}
	return true;
}


bool ServiceBanner::check() const
{
	switch ( tokens_ )	{
	case PRODUCT_NAME:
	case VERSION_MAJOR:
	case VERSION_MINOR:
	case VERSION_REVISION:
	case PRODUCT_OS:
	case NONE:
	case UNDEFINED:
		return true;
	default:
		LOG_ERROR << "Unknown value for ServiceBanner::tokens: " << (int)tokens_;
		return false;
	}
	// for stupid compilers
}


void ServiceBanner::print( std::ostream& os ) const
{
	os << displayName() << std::endl;
	os << "   Service banner: ";
	switch ( tokens_ )	{
	case PRODUCT_NAME:	os << "product name only"; break;
	case VERSION_MAJOR:	os << "product name and major version"; break;
	case VERSION_MINOR:	os << "product name and minor version"; break;
	case VERSION_REVISION:	os << "product name and revision"; break;
	case PRODUCT_OS:	os << "product name, revision and OS"; break;
	case NONE:		os << "none"; break;
	case UNDEFINED:
	default:		os << "NOT DEFINED !"; break;
	}
	os << std::endl;
	os << "   Print service name: " << ( serverName_ ? "yes" : "no" ) << std::endl;
}


std::string ServiceBanner::toString() const
{
	std::string	banner;

	switch ( tokens_ )	{
	case PRODUCT_NAME:	banner = "Wolframe"; break;
	case VERSION_MAJOR:	banner = "Wolframe 0"; break;
	case VERSION_MINOR:	banner = "Wolframe 0.0"; break;
	case VERSION_REVISION:	banner = "Wolframe 0.0.4"; break;
	case PRODUCT_OS:	banner = "Wolframe 0.0.4 on Linux"; break;
	case NONE:		break;
	case UNDEFINED:
	default:		throw std::domain_error( "ServiceBanner: unknown ServerTokens value" );
	}
	if ( serverName_ )
		banner = "GogoServer " + banner;

	return banner;
}

}} // namespace _Wolframe::Configuration
