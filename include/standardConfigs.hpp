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
namespace net	{

	/// network server configuration
	struct Configuration : public _Wolframe::config::OLD_ConfigurationBase
	{
	public:
		unsigned short		threads;
		unsigned short		maxConnections;

		// listen on
		std::list<net::ServerTCPendpoint> address;
	#ifdef WITH_SSL
		std::list<net::ServerSSLendpoint> SSLaddress;
	#endif // WITH_SSL

		/// constructor
		Configuration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

		void setCanonicalPathes( const std::string& referencePath );

		//			Not implemented yet, inherited from base for the time being
		//			bool test() const;
	};

} // namespace net

namespace log	{

	/// logger configuration
	struct LoggerConfiguration : public _Wolframe::config::OLD_ConfigurationBase
	{
	public:
		bool			logToStderr;
		LogLevel::Level		stderrLogLevel;

		bool			logToFile;
		std::string		logFile;
		LogLevel::Level		logFileLogLevel;
		std::string		logFileIdent;
		bool			logToSyslog;
		SyslogFacility::Facility syslogFacility;
		LogLevel::Level		syslogLogLevel;
		std::string		syslogIdent;
#if defined( _WIN32 )
		bool			logToEventlog;
		std::string		eventlogLogName;
		std::string		eventlogSource;
		LogLevel::Level eventlogLogLevel;
#endif // defined( _WIN32 )

		/// constructor
		LoggerConfiguration();

		/// methods
		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

		void setCanonicalPathes( const std::string& referencePath );

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

		void foreground( LogLevel::Level debugLevel, bool useConfig );
	};

} // namespace log

namespace config	{

	/// daemon / service configuration
	struct ServiceConfiguration : public _Wolframe::config::OLD_ConfigurationBase
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
		void print( std::ostream& os, size_t indent ) const;

//			Not implemented yet, inherited from base for the time being
//			bool test() const;

#if !defined( _WIN32 )
		void setCanonicalPathes( const std::string& referencePath );
		void override( const std::string& user, const std::string& group );
#endif // !defined( _WIN32 )
	};


	/// Service signature
	class ServiceBanner : public _Wolframe::config::OLD_ConfigurationBase
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
		ServiceBanner() : OLD_ConfigurationBase( "Service Banner", NULL, "Service banner" ),
						tokens_( UNDEFINED ),
						serverName_( false ),
						serverNameDefined_( false )	{}
		/// methods
		std::string toString() const;

		bool parse( const boost::property_tree::ptree& pt, const std::string& node );
		bool check() const;
		void print( std::ostream& os, size_t indent ) const;

//		Not implemented yet, inherited from base for the time being
//		bool test() const;
	private:
		/// data members
		SignatureTokens	tokens_;
		bool		serverName_;
		bool		serverNameDefined_;
	};


	/// logger configuration
	struct LoggerConfiguration : public _Wolframe::config::OLD_ConfigurationBase
	{
	public:
		bool			logToStderr;
		log::LogLevel::Level	stderrLogLevel;

		bool	 // namespace
	logToFile;
		std::string		logFile;
		log::LogLevel::Level	logFileLogLevel;
		std::string		logFileIdent;
		bool			logToSyslog;
		log::SyslogFacility::Facility syslogFacility;
		log::LogLevel::Level	syslogLogLevel;
		std::string		syslogIdent;
#if defined( _WIN32 )
		bool			logToEventlog;
		std::string		eventlogLogName;
		std::string		eventlogSource;
		log::LogLevel::Level	eventlogLogLevel;
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

		void foreground( log::LogLevel::Level debugLevel, bool useConfig );
	};

} // namespace config
} // namespace _Wolframe

#endif // _STANDARD_CONFIGS_HPP_INCLUDED
