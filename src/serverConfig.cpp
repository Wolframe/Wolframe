/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"
#include "appProperties.hpp"
#include "logger/logger-v1.hpp"

#include <boost/filesystem.hpp>
#include "utils/fileUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>

namespace _Wolframe {
namespace net {

static const unsigned short DEFAULT_NOF_THREADS = 4;

/// Parse the configuration
bool Configuration::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			   const module::ModulesDirectory* /*modules*/ )
{
	bool retVal = true;

	bool threadsDefined, maxConnDefined;
	threadsDefined = maxConnDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
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
			std::string	identifier;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			bool portDefined, connDefined;
			portDefined = connDefined = false;

			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			      L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "host" ) ||
						boost::algorithm::iequals( L2it->first, "address" ))	{
					bool isDefined = ( ! host.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, host, &isDefined ))
						retVal = false;
					else	{
						if ( host == "*" )
							host = "0.0.0.0";
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, port,
								config::Parser::RangeDomain<unsigned short>( 1 ),
								&portDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "identifier" ))	{
					bool isDefined = ( ! identifier.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, identifier, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, maxConn, &connDefined ))
						retVal = false;
				}
				else
					LOG_WARNING << logPrefix() << "socket: unknown configuration option: '"
						    << L2it->first << "'";
			}
			if ( port == 0 )
				port = net::defaultTCPport();

			net::ServerTCPendpoint lep( host, port, identifier, maxConn );
			address.push_back( lep );
		}
		else if ( boost::algorithm::iequals( L1it->first, "SSLsocket" ))	{
			std::string	host;
			std::string	identifier;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			std::string	certFile;
			std::string	keyFile;
			std::string	CAdirectory;
			std::string	CAchainFile;
			bool		verify = false;
			bool portDefined, connDefined, verifyDefined;
			portDefined = connDefined = verifyDefined = false;

			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
			      L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "host" ) ||
						boost::algorithm::iequals( L2it->first, "address" ))	{
					bool isDefined = ( ! host.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, host, &isDefined ))
						retVal = false;
					else	{
						if ( host == "*" )
							host = "0.0.0.0";
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, port,
								config::Parser::RangeDomain<unsigned short>( 1 ),
								&portDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "identifier" ))	{
					bool isDefined = ( ! identifier.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, identifier, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, maxConn, &connDefined ))
						retVal = false;
				}

				else if ( boost::algorithm::iequals( L2it->first, "certificate" ))	{
					bool isDefined = ( ! certFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, certFile, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "key" ))	{
					bool isDefined = ( ! keyFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, keyFile, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAdirectory" ))	{
					bool isDefined = ( ! CAdirectory.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, CAdirectory, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAchainFile" ))	{
					bool isDefined = ( ! CAchainFile.empty());
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, CAchainFile, &isDefined ))
						retVal = false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "verify" ))	{
					if ( ! config::Parser::getValue( logPrefix().c_str(), *L2it, verify,
								 config::Parser::BoolDomain(), &verifyDefined ))
						return false;
				}
				else
					LOG_WARNING << logPrefix() << "SSLsocket: unknown configuration option: '"
						    << L2it->first << "'";
			}
#ifdef WITH_SSL
			if ( port == 0 )
				port = net::defaultSSLport();

			net::ServerSSLendpoint lep( host, port, identifier, maxConn,
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
		os << "   Unencrypted: " << it->toString() << ", identifier '" << it->identifier() << "'";
		if ( it->maxConnections() != 0 )
			os << ", maximum " << it->maxConnections() << " client connections";
		os << std::endl;

		for ( ++it; it != address.end(); ++it )	{
			os << "                " << it->toString() << ", identifier '" << it->identifier() << "'";
			if ( it->maxConnections() != 0 )
				os << ", maximum " << it->maxConnections() << " client connections";
			os << std::endl;
		}
	}
#ifdef WITH_SSL
	if ( SSLaddress.size() > 0 )	{
		std::list<net::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
		os << "           SSL: " << it->toString() << ", identifier '" << it->identifier() << "'";
		if ( it->maxConnections() != 0 )
			os << ", maximum " << it->maxConnections() << " client connections";
		os << std::endl;

		os << "                   certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
		os << "                   key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
		os << "                   CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
		os << "                   CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
		os << "                   verify client certificate: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
		for ( ++it; it != SSLaddress.end(); ++it )	{
			os << "                " << it->toString() << ", identifier '" << it->identifier() << "'";
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
