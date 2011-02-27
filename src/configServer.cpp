//
// server configuration
//

#include "standardConfigs.hpp"
#include "configHelpers.hpp"
#include "appProperties.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _Wolframe	{
	namespace	Configuration	{

// Constructor
ServerConfiguration::ServerConfiguration( const std::string& node, const std::string& header )
	: ConfigurationBase( node, header )
{
	threads = 0;
	maxConnections = 0;
}


// Server configuration functions
	void ServerConfiguration::print( std::ostream& os ) const
	{
		// Unix daemon
#if !defined(_WIN32)
		os << "Run as " << (user.empty() ? "(not specified)" : user) << ":"
				<< (group.empty() ? "(not specified)" : group) << std::endl;
		os << "PID file: " << pidFile << std::endl;
#else
		// Windows service
		os << "When run as service" << std::endl
				<< "  Name: " << serviceName << std::endl
				<< "  Displayed name: " << serviceDisplayName << std::endl
				<< "  Description: " << serviceDescription << std::endl;
#endif
		os << "Number of threads: " << threads << std::endl;
		os << "Maximum number of connections: " << maxConnections << std::endl;

		os << "Network" << std::endl;
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
	}

	/// Check if the server configuration makes sense
	bool ServerConfiguration::check( std::ostream& os ) const
	{
		bool	correct = true;

		for ( std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
									it != SSLaddress.end(); ++it )	{
			// if it listens to SSL a certificate file and a key file are required
			if ( it->certificate().empty() )	{
				os << "No SSL certificate specified for " << it->toString() << std::endl;
				correct = false;
			}
			if ( it->key().empty() )	{
				os << "No SSL key specified for " << it->toString() << std::endl;
				correct = false;
			}
			// verify client SSL certificate needs either certificate dir or chain file
			if ( it->verifyClientCert() && it->CAdirectory().empty() && it->CAchain().empty() )	{
				os << "Client SSL certificate verification requested but no CA "
						"directory or CA chain file specified for "
						<< it->toString() << std::endl;
				correct = false;
			}
		}
		return correct;
	}


/// Parse the configuration
bool ServerConfiguration::parse( boost::property_tree::ptree& pt, std::ostream& os )
{
	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "threads" ))	{
			if ( ! getUnsignedShortValue( L1it, displayStr(), "threads", threads, os ))
				return false;
		}
#if !defined(_WIN32)
		else if ( boost::algorithm::iequals( L1it->first, "daemon" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "user" ))	{
					if ( ! getStringValue( L2it, displayStr(), "user", user, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "group" ))	{
					if ( ! getStringValue( L2it, displayStr(), "group", group, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "pidFile" ))	{
					if ( ! getStringValue( L2it, displayStr(), "pidFile", pidFile, os ))
						return false;
				}
				else	{
					os << displayStr() << ": daemon: unknown configuration option: <" << L2it->first << ">";
					return false;
				}
			}
		}
#endif
#if defined(_WIN32)
		else if ( boost::algorithm::iequals( L1it->first, "service" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "serviceName" ))	{
					if ( ! getStringValue( L2it, displayStr(), "serviceName", serviceName, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "displayName" ))	{
					if ( ! getStringValue( L2it, displayStr(), "displayName", serviceDisplayName, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L1it->first, "description" ))	{
					if ( ! getStringValue( L2it, displayStr(), "description", serviceDescription, os ))
						return false;
				}
				else	{
					os << displayStr() << ": service: unknown configuration option: <" << L2it->first << ">";
					return false;
				}
			}
			if ( serviceName.empty() )
				serviceName = defaultServiceName();
			if ( serviceDisplayName.empty() )
				serviceDisplayName = defaultServiceDisplayName();
			if ( serviceDescription.empty() )
				serviceDescription = defaultServiceDescription();
		}
#endif
		else if ( boost::algorithm::iequals( L1it->first, "listen" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! getUnsignedShortValue( L2it, displayStr(), "maxConnections", maxConnections, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L2it->first, "socket" ))	{
					std::string	host;
					unsigned short	port = 0;
					unsigned short	maxConn = 0;
					for ( boost::property_tree::ptree::const_iterator L3it = L2it->second.begin();
											L3it != L2it->second.end(); L3it++ )	{
						if ( boost::algorithm::iequals( L3it->first, "host" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "host", host, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "address" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "address", host, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "port" ))	{
							if ( ! getUnsignedShortValue( L3it, displayStr(), "port", port, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "maxConnections" ))	{
							if ( ! getUnsignedShortValue( L3it, displayStr(), "maxConnections", maxConn, os ))
								return false;
						}
						else	{
							os << displayStr() << ": socket: unknown configuration option: <" << L3it->first << ">";
							return false;
						}
					}
					if ( port == 0 )
						port = defaultTCPport();

					Network::ServerTCPendpoint lep( host, port, maxConn );
					address.push_back( lep );
				}
				else if ( boost::algorithm::iequals( L2it->first, "SSLsocket" ))	{
					std::string	host;
					unsigned short	port = 0;
					unsigned short	maxConn = 0;
					std::string	certFile;
					std::string	keyFile;
					std::string	CAdirectory;
					std::string	CAchainFile;
					bool		verify;
					for ( boost::property_tree::ptree::const_iterator L3it = L2it->second.begin();
											L3it != L2it->second.end(); L3it++ )	{
						if ( boost::algorithm::iequals( L3it->first, "host" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "host", host, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "address" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "address", host, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "port" ))	{
							if ( ! getUnsignedShortValue( L3it, displayStr(), "port", port, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "maxConnections" ))	{
							if ( ! getUnsignedShortValue( L3it, displayStr(), "maxConnections", maxConn, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "certificate" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "certificate", certFile, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "key" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "key", keyFile, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "CAdirectory" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "CAdirectory", CAdirectory, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "CAchainFile" ))	{
							if ( ! getHostnameValue( L3it, displayStr(), "CAchainFile", CAchainFile, os ))
								return false;
						}
						else if ( boost::algorithm::iequals( L3it->first, "verify" ))	{
							if ( ! getBoolValue( L3it, displayStr(), "verify", verify, os ))
								return false;
						}
						else	{
							os << displayStr() << ": socket: unknown configuration option: <" << L3it->first << ">";
							return false;
						}
					}
					if ( port == 0 )
						port = defaultSSLport();

					Network::ServerSSLendpoint lep( host, port, maxConn,
									certFile, keyFile,
									verify, CAdirectory, CAchainFile );
					SSLaddress.push_back( lep );
				}
				else	{
					os << displayStr() << ": listen: unknown configuration option: <" << L2it->first << ">";
					return false;
				}
			}
		}
		else	{
			os << displayStr() << ": unknown configuration option: <" << L1it->first << ">";
			return false;
		}
	}
	return true;
}


#if !defined(_WIN32)
	/// Override the server configuration with command line arguments
	void ServerConfiguration::override( const std::string& usr, const std::string& grp )
	{
		if ( !usr.empty())
			user = usr;
		if ( !grp.empty())
			group = grp;
	}
#endif


	} // namespace Configuration
} // namespace _Wolframe
