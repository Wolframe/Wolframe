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
// standard configuration structures
//

#ifndef _STANDARD_CONFIGS_HPP_INCLUDED
#define _STANDARD_CONFIGS_HPP_INCLUDED

#include "configurationBase.hpp"
#include "serverEndpoint.hpp"
#include "logger/logLevel.hpp"

#include "logger/logSyslogFacility.hpp"

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
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;

		void setCanonicalPathes( const std::string& referencePath );

		//			Not implemented yet, inherited from base for the time being
		//			bool test() const;
	};

} // namespace Network

namespace Logging	{

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
		bool			logToSyslog;
		Logging::SyslogFacility::Facility syslogFacility;
		Logging::LogLevel::Level		syslogLogLevel;
		std::string		syslogIdent;
#if defined( _WIN32 )
		bool			logToEventlog;
		std::string		eventlogLogName;
		std::string		eventlogSource;
		Logging::LogLevel::Level eventlogLogLevel;
#endif // defined( _WIN32 )

		/// constructor
		LoggerConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;

		void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

		void foreground( Logging::LogLevel::Level debugLevel, bool useConfig );
	};

} // namespace Logging

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
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
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
	class ServiceBanner : public _Wolframe::Configuration::ConfigurationBase
	{
	public:
		enum SignatureTokens	{
			PRODUCT_NAME,
			VERSION_MAJOR,
			VERSION_MINOR,
			VERSION_REVISION,
			PRODUCT_OS,
			NONE,
			UNDEFINED
		};

		/// constructor
		ServiceBanner() : ConfigurationBase( "Service Banner" ),
						tokens_( UNDEFINED ),
						serverName_( false ),
						serverNameDefined_( false )	{}

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;
		std::string toString() const;
	private:
		/// data members
		SignatureTokens	tokens_;
		bool		serverName_;
		bool		serverNameDefined_;
	};


		/// logger configuration
		struct LoggerConfiguration : public _Wolframe::Configuration::ConfigurationBase
		{
		public:
			bool			logToStderr;
			Logging::LogLevel::Level		stderrLogLevel;

			bool	 // namespace
		logToFile;
			std::string		logFile;
			Logging::LogLevel::Level		logFileLogLevel;
			std::string		logFileIdent;
			bool			logToSyslog;
			Logging::SyslogFacility::Facility syslogFacility;
			Logging::LogLevel::Level		syslogLogLevel;
			std::string		syslogIdent;
#if defined( _WIN32 )
			bool			logToEventlog;
			std::string		eventlogLogName;
			std::string		eventlogSource;
			Logging::LogLevel::Level eventlogLogLevel;
#endif // defined( _WIN32 )

			/// constructor
			LoggerConfiguration();

			/// methods
			bool parse( const boost::property_tree::ptree& pt, const std::string& node );
			bool check() const;
			void print( std::ostream& os ) const;

			void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

			void foreground( Logging::LogLevel::Level debugLevel, bool useConfig );
		};

	} // namespace Configuration

} // namespace _Wolframe

#endif // _STANDARD_CONFIGS_HPP_INCLUDED
