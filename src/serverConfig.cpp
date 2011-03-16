//
// server configuration
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "appProperties.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


using namespace _Wolframe::Configuration;

namespace _Wolframe	{
	namespace Network	{

// Constructor
ServerConfiguration::ServerConfiguration()
	: ConfigurationBase( "Network Server" )
{
	threads = 0;
	maxConnections = 0;
}


// Server configuration functions
	void ServerConfiguration::print( std::ostream& os ) const
	{
		os << displayName() << std::endl;
		os << "   Number of client threads: " << threads << std::endl;
		os << "   Maximum number of connections (global): " << maxConnections << std::endl;

		if ( address.size() > 0 )	{
			std::list<Network::ServerTCPendpoint>::const_iterator it = address.begin();
			os << "   Unencrypted: " << it->toString();
			if ( it->maxConnections() != 0 )
				os << ", maximum: " << it->maxConnections() << " client connections";
			os << std::endl;

			for ( ++it; it != address.end(); ++it )	{
				os << "                " << it->toString();
				if ( it->maxConnections() != 0 )
					os << ", maximum: " << it->maxConnections() << " client connections";
				os << std::endl;
			}
		}
#ifdef WITH_SSL
		if ( SSLaddress.size() > 0 )	{
			std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
			os << "           SSL: " << it->toString();
			if ( it->maxConnections() != 0 )
				os << ", maximum: " << it->maxConnections() << " client connections";
			os << std::endl;

			os << "                   certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
			os << "                   key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
			os << "                   CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
			os << "                   CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
			os << "                   verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			for ( ++it; it != SSLaddress.end(); ++it )	{
				os << "                " << it->toString();
				if ( it->maxConnections() != 0 )
					os << ", maximum: " << it->maxConnections() << " client connections";
				os << std::endl;
				os << "                   certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
				os << "                   key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
				os << "                   CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
				os << "                   CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
				os << "                   verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			}
		}
#endif // WITH_SSL
	}

	/// Check if the server configuration makes sense
#ifdef WITH_SSL
	bool ServerConfiguration::check() const
	{
		bool correct = true;
		for ( std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
										it != SSLaddress.end(); ++it )	{
			// if it listens to SSL a certificate file and a key file are required
			if ( it->certificate().empty() )	{
				LOG_ERROR << "No SSL certificate specified for " << it->toString() << std::endl;
				correct = false;
			}
			if ( it->key().empty() )	{
				LOG_ERROR << "No SSL key specified for " << it->toString() << std::endl;
				correct = false;
			}
			// verify client SSL certificate needs either certificate dir or chain file
			if ( it->verifyClientCert() && it->CAdirectory().empty() && it->CAchain().empty() )	{
				LOG_ERROR << "Client SSL certificate verification requested but no CA "
				      "directory or CA chain file specified for "
				   << it->toString() << std::endl;
				correct = false;
			}
		}
		return correct;
	}
#else
	bool ServerConfiguration::check() const
	{
		return true;
	}
#endif // WITH_SSL


/// Parse the configuration
bool ServerConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /* node */ )
{
	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "threads" ))	{
			if ( ! getUnsignedShortValue( L1it->second, displayName(), L1it->first, threads ))
				return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "maxConnections" ))	{
			if ( ! getUnsignedShortValue( L1it->second, displayName(), L1it->first, maxConnections ))
				return false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "socket" ))	{
			std::string	host;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "host" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "host", host ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "address" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "address", host ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! getUnsignedShortValue( L2it->second, displayName(), "port", port ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! getUnsignedShortValue( L2it->second, displayName(), "maxConnections", maxConn ))
						return false;
				}
				else	{
					LOG_ERROR << displayName() << ": socket: unknown configuration option: <" << L2it->first << ">";
					return false;
				}
			}
			if ( port == 0 )
				port = defaultTCPport();

			Network::ServerTCPendpoint lep( host, port, maxConn );
			address.push_back( lep );
		}
		else if ( boost::algorithm::iequals( L1it->first, "SSLsocket" ))	{
			std::string	host;
			unsigned short	port = 0;
			unsigned short	maxConn = 0;
			std::string	certFile;
			std::string	keyFile;
			std::string	CAdirectory;
			std::string	CAchainFile;
			bool		verify = false;
			bool		verifySet = false;
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
										L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "host" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "host", host ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "address" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "address", host ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "port" ))	{
					if ( ! getUnsignedShortValue( L2it->second, displayName(), "port", port ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! getUnsignedShortValue( L2it->second, displayName(), "maxConnections", maxConn ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "certificate" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "certificate", certFile ))
						return false;
					if ( ! boost::filesystem::path( certFile ).is_absolute() )
						LOG_WARNING << displayName() << ": certificate file is not absolute: "
							    << certFile << std::endl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "key" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "key", keyFile ))
						return false;
					if ( ! boost::filesystem::path( keyFile ).is_absolute() )
						LOG_WARNING << displayName() << ": key file is not absolute: "
							    << keyFile << std::endl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAdirectory" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "CAdirectory", CAdirectory ))
						return false;
					if ( ! boost::filesystem::path( CAdirectory ).is_absolute() )
						LOG_WARNING << displayName() << ": CA directory is not absolute: "
							    << CAdirectory << std::endl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "CAchainFile" ))	{
					if ( ! getHostnameValue( L2it->second, displayName(), "CAchainFile", CAchainFile ))
						return false;
					if ( ! boost::filesystem::path( CAchainFile ).is_absolute() )
						LOG_WARNING << displayName() << ": CA chain file is not absolute: "
							    << CAchainFile << std::endl;
				}
				else if ( boost::algorithm::iequals( L2it->first, "verify" ))	{
					if ( ! getBoolValue( L2it->second, displayName(), "verify", verify, verifySet ))
						return false;
				}
				else	{
					LOG_WARNING << displayName() << ": SSLsocket: unknown configuration option: <"
						    << L2it->first << ">";
//					return false;
				}
			}
#ifdef WITH_SSL
			if ( port == 0 )
				port = defaultSSLport();

			Network::ServerSSLendpoint lep( host, port, maxConn,
							certFile, keyFile,
							verify, CAdirectory, CAchainFile );
			SSLaddress.push_back( lep );
#endif // WITH_SSL
		}
		else	{
			LOG_WARNING << displayName() << ": unknown configuration option: <" << L1it->first << ">";
//			return false;
		}
	}
	return true;
}

#ifdef WITH_SSL
void ServerConfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<Network::ServerSSLendpoint>::iterator it = SSLaddress.begin(); it != SSLaddress.end(); ++it )
		it->setAbsolutePath( refPath );
}
#else
void ServerConfiguration::setCanonicalPathes( const std::string& /* refPath */ )
{
}
#endif // WITH_SSL

	} // namespace Network
} // namespace _Wolframe
