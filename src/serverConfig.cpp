/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
// server configuration
//

#include "standardConfigs.hpp"
#include "server.hpp"
#include "config/configurationTree.hpp"
#include "config/valueParser.hpp"
#include "appProperties.hpp"
#include "logger-v1.hpp"
#include "types/addressRestriction.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "utils/fileUtils.hpp"
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe {
namespace net {

static const unsigned short DEFAULT_NOF_THREADS = 4;

static bool parseAddressRestriction( types::AddressRestriction& restr, const config::ConfigurationNode& pt)
{
	bool success = true;
	bool orderDefined = false;
	bool allowDefined = false;
	bool denyDefined = false;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )
	{
		if ( boost::algorithm::iequals( L1it->first, "allow" ))
		{
			allowDefined = true;
			if (boost::algorithm::iequals( L1it->second.data(), "all")
			||	boost::algorithm::iequals( L1it->second.data(), "*"))
			{
				restr.defineAllowedAll();
			}
			else
			{
				try
				{
					restr.defineAddressAllowed( L1it->second.data());
				}
				catch (const std::runtime_error& e)
				{
					success = false;
					LOG_ERROR << "Error in deny address restriction " << L1it->second.position().logtext() << ": " << e.what();
				}
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "deny" ))
		{
			denyDefined = true;
			if (boost::algorithm::iequals( L1it->second.data(), "all")
			||	boost::algorithm::iequals( L1it->second.data(), "*"))
			{
				restr.defineDeniedAll();
			}
			else
			{
				try
				{
					restr.defineAddressDenied( L1it->second.data());
				}
				catch (const std::runtime_error& e)
				{
					success = false;
					LOG_ERROR << "Error in deny address restriction " << L1it->second.position().logtext() << ": " << e.what();
				}
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "order" ))
		{
			orderDefined = true;
			std::string orderval;
			std::string::const_iterator vi = L1it->second.data().begin(), ve = L1it->second.data().end();
			for (; vi != ve; ++vi)
			{
				if ((unsigned char)*vi > 32)
				{
					orderval.push_back( *vi);
				}
			}
			if (boost::algorithm::iequals( orderval, "allow,deny"))
			{
				restr.defineOrder( types::AddressRestriction::Allow_Deny);
			}
			else if (boost::algorithm::iequals( orderval, "deny,allow"))
			{
				restr.defineOrder( types::AddressRestriction::Deny_Allow);
			}
			else
			{
				success = false;
				LOG_ERROR << "unknown value specified for order (neither 'deny,allow' nor 'allow,deny'): '" << orderval << "' " << L1it->second.position().logtext();
			}
		}
	}
	if (!orderDefined)
	{
		if (denyDefined)
		{
			success = false;
			LOG_ERROR << "restrictions with 'deny' defined but without 'order' " << pt.position().logtext();
		}
		if (allowDefined)
		{
			restr.defineOrder( types::AddressRestriction::Allow_Deny);
		}
	}
	if (!allowDefined)
	{
		LOG_WARNING << "restrictions without allow will exclude all " << pt.position().logtext();
	}
	return success;
}

/// Parse the configuration
bool Configuration::parse( const config::ConfigurationNode& pt, const std::string& /*node*/,
			   const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;
	bool threadsDefined, maxConnDefined;
	threadsDefined = maxConnDefined = false;
	types::PropertyTree::Position position;

	for ( config::ConfigurationNode::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		position = L1it->second.position();
		if ( boost::algorithm::iequals( L1it->first, "threads" ))	{
			if ( ! config::Parser::getValue( logPrefix().c_str(), *L1it, threads,
						 config::Parser::RangeDomain<unsigned short>( 1 ), &threadsDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "maxConnections" ))	{
			if ( ! config::Parser::getValue( logPrefix().c_str(), *L1it, maxConnections,
						 &maxConnDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "socket" ))	{
			std::string	host;
			net::LocalEndpointConfig localEndpointConfig;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			bool portDefined, connDefined;
			portDefined = connDefined = false;
			types::AddressRestriction restrictions;
			bool restrictionsDefined = false;

			for ( config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
			      L2it != L1it->second.end(); L2it++ )	{

				position = L2it->second.position();
				if ( boost::algorithm::iequals( L2it->first, "host" ) ||
						boost::algorithm::iequals( L2it->first, "address" ))	{
					bool isDefined = ( ! host.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, host, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
					else	{
						if ( host == "*" )
							host = "0.0.0.0";
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, port,
								config::Parser::RangeDomain<unsigned short>( 1 ),
								&portDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "identifier" ))	{
					bool isDefined = ( ! localEndpointConfig.socketIdentifier.empty());
					std::string val;
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, val, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
					localEndpointConfig.socketIdentifier = val;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, maxConn, &connDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if (boost::algorithm::iequals( L2it->first, "restrictions" ))	{
					if (restrictionsDefined)
					{
						LOG_ERROR << "Duplicate definition of restrictions in socket definition " << position.logtext();
						retVal = false;
					}
					restrictionsDefined = true;
					retVal &= parseAddressRestriction( restrictions, L2it->second);
				}
				else
					LOG_WARNING << logPrefix() << "socket: unknown configuration option: '"
						    << L2it->first << "' " << position.logtext();
			}
			if ( port == 0 )
				port = net::defaultTCPport();
			if (!restrictionsDefined)
			{
				LOG_WARNING << "No restrictions defined for socket " << position.logtext() << " setting default to allow all " << position.logtext();
				restrictions.defineAllowedAll();
			}
			net::ServerTCPendpoint lep( host, port, maxConn, localEndpointConfig, restrictions );
			address.push_back( lep );
		}
		else if ( boost::algorithm::iequals( L1it->first, "SSLsocket" ))	{
			std::string	host;
			net::LocalEndpointConfig localEndpointConfig;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			std::string	certFile;
			std::string	keyFile;
			std::string	CAdirectory;
			std::string	CAchainFile;
			bool		verify = false;
			bool portDefined, connDefined, verifyDefined;
			portDefined = connDefined = verifyDefined = false;
			types::AddressRestriction restrictions;
			bool restrictionsDefined;

			for ( config::ConfigurationNode::const_iterator L2it = L1it->second.begin();
			      L2it != L1it->second.end(); L2it++ )	{
				position = L2it->second.position();
				if ( boost::algorithm::iequals( L2it->first, "host" ) ||
						boost::algorithm::iequals( L2it->first, "address" ))	{
					bool isDefined = ( ! host.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, host, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
					else	{
						if ( host == "*" )
							host = "0.0.0.0";
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, port,
								config::Parser::RangeDomain<unsigned short>( 1 ),
								&portDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "identifier" ))	{
					bool isDefined = ( ! localEndpointConfig.socketIdentifier.empty());
					std::string val;
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, val, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
					localEndpointConfig.socketIdentifier = val;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, maxConn, &connDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if (boost::algorithm::iequals( L2it->first, "restrictions" ))	{
					if (restrictionsDefined)
					{
						LOG_ERROR << "Duplicate definition of restrictions in socket definition " << position.logtext();
						retVal = false;
					}
					restrictionsDefined = true;
					retVal &= parseAddressRestriction( restrictions, L2it->second);
				}
				else if ( boost::algorithm::iequals( L2it->first, "certificate" ))	{
					bool isDefined = ( ! certFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, certFile, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "key" ))	{
					bool isDefined = ( ! keyFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, keyFile, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAdirectory" ))	{
					bool isDefined = ( ! CAdirectory.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, CAdirectory, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAchainFile" ))	{
					bool isDefined = ( ! CAchainFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, CAchainFile, &isDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "verify" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, verify,
								 config::Parser::BoolDomain(), &verifyDefined ))
					{
						LOG_ERROR << "Error in configuration file " << position.logtext();
						retVal = false;
					}
				}
				else
					LOG_WARNING << logPrefix() << "SSLsocket: unknown configuration option: '"
						    << L2it->first << "'" << position.logtext();
			}
#ifdef WITH_SSL
			if ( port == 0 )
				port = net::defaultSSLport();
			if (!restrictionsDefined)
			{
				LOG_WARNING << "No restrictions defined for socket at " << position.logtext() << " setting default to allow all " << position.logtext();
				restrictions.defineAllowedAll();
			}

			net::ServerSSLendpoint lep( host, port, maxConn,
						    localEndpointConfig, restrictions,
						    certFile, keyFile,
						    verify, CAdirectory, CAchainFile );
			SSLaddress.push_back( lep );
#else
			LOG_WARNING << logPrefix() << "configuration of SSLsocket ignored (SSL support not enabled)";
#endif // WITH_SSL
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '" << L1it->first << "'";
		}
	}
	if ( threads == 0 )
		threads = DEFAULT_NOF_THREADS;
	return retVal;
}


// Constructor
Configuration::Configuration()
	: ConfigurationBase( "Network Server", NULL, "Network server configuration" )
{
	threads = 0;
	maxConnections = 0;
}


// Server configuration functions
void Configuration::print( std::ostream& os, size_t /* indent */ ) const
{
	os<< sectionName() << std::endl;
	os << "   Number of client threads: " << threads << std::endl;
	if ( maxConnections > 0 )
		os << "   Maximum number of connections (global): " << maxConnections << std::endl;

	if ( address.size() > 0 )	{
		std::list<net::ServerTCPendpoint>::const_iterator it = address.begin();
		os << "   Unencrypted: " << it->toString() << ", identifier '" << it->config().socketIdentifier << "'";
		if ( it->maxConnections() != 0 )
			os << ", maximum " << it->maxConnections() << " client connections";
		os << std::endl;

		for ( ++it; it != address.end(); ++it )	{
			os << "                " << it->toString() << ", identifier '" << it->config().socketIdentifier << "'";
			if ( it->maxConnections() != 0 )
				os << ", maximum " << it->maxConnections() << " client connections";
			os << std::endl;
		}
	}
#ifdef WITH_SSL
	if ( SSLaddress.size() > 0 )	{
		std::list<net::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
		os << "           SSL: " << it->toString() << ", identifier '" << it->config().socketIdentifier << "'";
		if ( it->maxConnections() != 0 )
			os << ", maximum " << it->maxConnections() << " client connections";
		os << std::endl;

		os << "                   certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
		os << "                   key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
		os << "                   CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
		os << "                   CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
		os << "                   verify client certificate: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
		for ( ++it; it != SSLaddress.end(); ++it )	{
			os << "                " << it->toString() << ", identifier '" << it->config().socketIdentifier << "'";
			if ( it->maxConnections() != 0 )
				os << ", maximum " << it->maxConnections() << " client connections";
			os << std::endl;
			os << "                   certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
			os << "                   key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
			os << "                   CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
			os << "                   CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
			os << "                   verify client certificate: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
		}
	}
#endif // WITH_SSL
}

/// Check if the server configuration makes sense
bool Configuration::check() const
{
	bool correct = true;

	// check the interface definitions
	size_t interfaces = address.size();
#ifdef WITH_SSL
	interfaces += SSLaddress.size();
#endif // WITH_SSL
	if ( !( interfaces > 0 ) )	{
		correct = false;
		LOG_FATAL << "No server interfaces defined";
	}

	// check SSL consistency
#ifdef WITH_SSL
	for ( std::list<net::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
	      it != SSLaddress.end(); ++it )	{
		// if it listens to SSL a certificate file and a key file are required
		if ( it->certificate().empty() )	{
			LOG_ERROR << "No SSL certificate specified for " << it->toString();
			correct = false;
		}
		if ( it->key().empty() )	{
			LOG_ERROR << "No SSL key specified for " << it->toString();
			correct = false;
		}
		// verify client SSL certificate needs either certificate dir or chain file
		if ( it->verifyClientCert() && it->CAdirectory().empty() && it->CAchain().empty() )	{
			LOG_ERROR << "Client SSL certificate verification requested but no CA "
				     "directory or CA chain file specified for "
				  << it->toString();
			correct = false;
		}
	}
#endif // WITH_SSL
	return correct;
}


#ifdef WITH_SSL
void Configuration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<net::ServerSSLendpoint>::iterator it = SSLaddress.begin(); it != SSLaddress.end(); ++it )
		it->setAbsolutePath( refPath );
}
#else
void Configuration::setCanonicalPathes( const std::string& /* refPath */ )
{
}
#endif // WITH_SSL

}} // namespace _Wolframe::net
