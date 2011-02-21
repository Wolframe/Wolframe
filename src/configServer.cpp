//
// server configuration
//

#include "configStandard.hpp"
#include "configHelpers.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <string>
#include <ostream>


namespace _SMERP	{
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
			os << "  Unencrypted: " << it->toString() << std::endl;
			for ( ++it; it != address.end(); ++it )
				os << "               " << it->toString() << std::endl;
		}
		if ( SSLaddress.size() > 0 )	{
			std::list<Network::ServerSSLendpoint>::const_iterator it = SSLaddress.begin();
			os << "          SSL: " << it->toString() << std::endl;
			os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
			os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
			os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
			os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
			os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
			for ( ++it; it != SSLaddress.end(); ++it )	{
				os << "               " << it->toString() << std::endl;
				os << "                  certificate: " << (it->certificate().empty() ? "(none)" : it->certificate()) << std::endl;
				os << "                  key file: " << (it->key().empty() ? "(none)" : it->key()) << std::endl;
				os << "                  CA directory: " << (it->CAdirectory().empty() ? "(none)" : it->CAdirectory()) << std::endl;
				os << "                  CA chain file: " << (it->CAchain().empty() ? "(none)" : it->CAchain()) << std::endl;
				os << "                  verify client: " << (it->verifyClientCert() ? "yes" : "no") << std::endl;
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


//////////////////////////////////
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
			}
		}
#endif
#if defined(_WIN32)
		else if ( boost::algorithm::iequals( L1it->first, "listen" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L1it->first, "serviceName" ))	{
					if ( ! getStringValue( L1it, displayStr(), "serviceName", serviceName, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L1it->first, "displayName" ))	{
					if ( ! getStringValue( L1it, displayStr(), "displayName", displayName, os ))
						return false;
				}
				else if ( boost::algorithm::iequals( L1it->first, "description" ))	{
					if ( ! getStringValue( L1it, displayStr(), "description", description, os ))
						return false;
				}
			}
		}
#endif
		else if ( boost::algorithm::iequals( L1it->first, "listen" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "maxConnections" ))	{
					if ( ! getUnsignedShortValue( L2it, displayStr(), "maxConnections", maxConnections, os ))
						return false;
				}
			}
		}
	}
	return true;
}

//________________________________
//	BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child( "server.listen" ))	{
//		std::string hostStr = v.second.get<std::string>( "address", std::string() );
//		if ( hostStr.empty() )	{
//			errMsg_ = "Interface must be defined";
//			return false;
//		}
//		if ( hostStr == "*" )
//			hostStr = "0.0.0.0";
//		std::string portStr = v.second.get<std::string>( "port", std::string() );
//		if ( portStr.empty() )	{
//			if ( v.first == "socket" )
//				port = DEFAULT_PORT;
//			else
//				port = SSL_DEFAULT_PORT;
//		}
//		else	{
//			try	{
//				port = boost::lexical_cast<unsigned short>( portStr );
//			}
//			catch( boost::bad_lexical_cast& )	{
//				errMsg_ = "Invalid value for port: ";
//				errMsg_ += portStr;
//				return false;
//			}
//			if ( port == 0 )	{
//				errMsg_ = "Port out of range: ";
//				errMsg_ += portStr;
//				return false;
//			}
//		}

//		unsigned maxConn = v.second.get<unsigned>( "maxConnections", 0 );

//		if ( v.first == "socket" )	{
//			Network::ServerTCPendpoint lep( hostStr, port, maxConn );
//			address.push_back( lep );
//		}
//		else if ( v.first == "SSLsocket" )	{
//// get SSL certificate / CA param
//			std::string certFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "certificate", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string keyFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "key", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string CAdirectory = boost::filesystem::absolute(
//								v.second.get<std::string>( "CAdirectory", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();
//			std::string CAchainFile = boost::filesystem::absolute(
//								v.second.get<std::string>( "CAchainFile", std::string() ),
//								boost::filesystem::path( file ).branch_path() ).string();

//			std::string tmpStr;
//			boost::logic::tribool flag = getBoolValue( v.second, "verify", tmpStr );
//			bool verify;
//			if ( flag )
//				verify = true;
//			else if ( !flag )
//				verify = false;
//			else	{
//				verify = true;
//				errMsg_ = "Unknown value \"";
//				errMsg_ += tmpStr;
//				errMsg_ += "\" for SSL verify client. WARNING: enabling verification";
//			}
//			Network::ServerSSLendpoint lep( hostStr, port, maxConn,
//							certFile, keyFile,
//							verify, CAdirectory, CAchainFile );
//			SSLaddress.push_back( lep );
//		}
//		else	{
//			errMsg_ = "Invalid listen type: ";
//			errMsg_ += v.first;
//			return false;
//		}
//	}
// ********************************

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
} // namespace _SMERP
