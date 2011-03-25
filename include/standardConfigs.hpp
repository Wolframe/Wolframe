//
// standard configuration structures
//

#ifndef _STANDARD_CONFIGS_HPP_INCLUDED
#define _STANDARD_CONFIGS_HPP_INCLUDED

#include "configurationBase.hpp"
#include "serverEndpoint.hpp"
#include "logger/logLevel.hpp"

#if !defined( _WIN32 )
#include "logger/logSyslogFacility.hpp"
#endif	// !defined( _WIN32 )

#include <string>
#include <list>


namespace _Wolframe {
	namespace Network	{

		/// network server configuration
		struct ServerConfiguration : public _Wolframe::Configuration::ConfigurationBase
		{
		public:
			unsigned short		threads;
			unsigned short		maxConnections;

			// listen on
			std::list<Network::ServerTCPendpoint> address;
#ifdef WITH_SSL
			std::list<Network::ServerSSLendpoint> SSLaddress;
#endif // WITH_SSL

			/// constructor
			ServerConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree::const_iterator it,
				    const std::string& node );
			bool check() const;
			void print( std::ostream& os ) const;

			void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
		};

	} // namespace Network


	namespace Configuration	{

	/// daemon / service configuration
	struct ServiceConfiguration : public _Wolframe::Configuration::ConfigurationBase
	{
	public:
#if !defined( _WIN32 )
		// daemon configuration
		std::string		user;
		std::string		group;
		std::string		pidFile;
#endif
#if defined( _WIN32 )
		// Windows service configuration
		std::string		serviceName;
		std::string		serviceDisplayName;
		std::string		serviceDescription;
#endif // !defined( _WIN32 )

		/// constructor
		ServiceConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree::const_iterator it,
			    const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

#if !defined( _WIN32 )
		void setCanonicalPathes( const std::string& referencePath );
		void override( const std::string& user, const std::string& group );
#endif // !defined( _WIN32 )
	};


	/// Service signature
	struct ServiceBanner : public _Wolframe::Configuration::ConfigurationBase
	{
		enum SignatureTokens	{
			PRODUCT_NAME,
			VERSION_MAJOR,
			VERSION_MINOR,
			VERSION_REVISION,
			PRODUCT_OS,
			NONE,
			UNDEFINED
		};
	public:
		/// data members
		SignatureTokens	tokens;
		bool		serverName;
		bool		serverNameDefined;

		/// constructor
		ServiceBanner() : ConfigurationBase( "Service Banner" ),
						tokens( UNDEFINED ),
						serverName( false ),
						serverNameDefined( false )	{}

		/// methods
		bool parse( const boost::property_tree::ptree::const_iterator it,
			    const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
		const std::string toString() const;
	};


		/// logger configuration
		struct LoggerConfiguration : public _Wolframe::Configuration::ConfigurationBase
		{
		public:
			bool			logToStderr;
			Logging::LogLevel::Level		stderrLogLevel;

			bool			logToFile;
			std::string		logFile;
			Logging::LogLevel::Level		logFileLogLevel;
			std::string		logFileIdent;
#if !defined( _WIN32 )
			bool			logToSyslog;
			Logging::SyslogFacility::Facility syslogFacility;
			Logging::LogLevel::Level		syslogLogLevel;
			std::string		syslogIdent;
#else
			bool			logToEventlog;
			std::string		eventlogLogName;
			std::string		eventlogSource;
			Logging::LogLevel::Level eventlogLogLevel;
#endif // !defined( _WIN32 )

			/// constructor
			LoggerConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree::const_iterator it,
				    const std::string& node );
			bool check() const;
			void print( std::ostream& os ) const;

			void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

			void foreground( Logging::LogLevel::Level debugLevel, bool useConfig );
		};


		/// database configuration
		struct DatabaseConfiguration : public _Wolframe::Configuration::ConfigurationBase
		{
		public:
			std::string		host;
			unsigned short		port;
			std::string		name;
			std::string		user;
			std::string		password;
			unsigned short		connections;
			unsigned short		acquireTimeout;

			/// constructor
			DatabaseConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree::const_iterator it,
				    const std::string& node );
			bool check() const;
			void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
		};

	} // namespace Configuration
} // namespace _Wolframe

#endif // _STANDARD_CONFIGS_HPP_INCLUDED
