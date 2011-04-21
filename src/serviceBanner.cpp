/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
//
// serviceBanner.cpp
//

#include "standardConfigs.hpp"
#include "config/valueParser.hpp"
#include "logger.hpp"
#include "appSingleton.hpp"

#include <string>
#include <stdexcept>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace config	{


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
bool ServiceBanner::parse( const boost::property_tree::ptree& pt, const std::string& node )
{

	if ( boost::algorithm::iequals( node, "ServerTokens" ))	{
		bool tokensDefined = ( tokens_ != UNDEFINED );
		std::string	val;
		if ( !Parser::getValue( sectionName().c_str(), node.c_str(), pt.get_value<std::string>(),
					val, &tokensDefined ))
			return false;
		tokens_ = strToToken( val );
		if ( tokens_ == UNDEFINED )	{
			LOG_ERROR << sectionName() << ": Unknown option '" << val << "' for " << node;
			return false;
		}
	}
	else if ( boost::algorithm::iequals( node, "ServerSignature" ))	{
		if ( !Parser::getValue( sectionName().c_str(), node.c_str(), pt.get_value<std::string>(),
				       serverName_, Parser::BoolDomain(), &serverNameDefined_ ))
			return false;
	}
	else	{
		LOG_FATAL << sectionName() << ": called with unknown configuration option: '"
			  << node << "'";
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


void ServiceBanner::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
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
	case PRODUCT_NAME:
		banner = "Wolframe";
		break;
	case VERSION_MAJOR:
		banner = "Wolframe " + _Wolframe::ApplicationSingleton::instance().version().toString( "version %M" );
		break;
	case VERSION_MINOR:
		banner = "Wolframe " + _Wolframe::ApplicationSingleton::instance().version().toString( "version %M.%m" );
		break;
	case VERSION_REVISION:
		banner = "Wolframe " + _Wolframe::ApplicationSingleton::instance().version().toString( "version %M.%m.%r" );
		break;
	case PRODUCT_OS:
		banner = "Wolframe " + _Wolframe::ApplicationSingleton::instance().version().toString( "version %M.%m.%r" ) + " OS";
		break;
	case NONE:		break;
	case UNDEFINED:
	default:		throw std::domain_error( "ServiceBanner: unknown ServerTokens value" );
	}
	if ( serverName_ )
		banner = "GogoServer " + banner;

	return banner;
}

}} // namespace _Wolframe::Configuration
