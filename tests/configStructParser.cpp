///
/// \brief configuration lexem parser unit tests using google test framework (gTest)
///

#include "config/description.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <iostream>
#include <boost/property_tree/info_parser.hpp>
#include "miscUtils.hpp"
#include "logger.hpp"

using namespace _Wolframe;
using namespace config;


struct LoggerConfig
{
	struct ToStderr
	{
		int loglevel;

		static const config::DescriptionBase* description();
	};

	struct ToFile
	{
		int loglevel;
		std::string filename;

		static const config::DescriptionBase* description();
	};

	struct ToSyslog
	{
		int loglevel;
		int facility;
		std::string ident;

		static const config::DescriptionBase* description();
	};

	struct ToEventlog
	{
		int loglevel;
		std::string name;
		std::string source;

		static const config::DescriptionBase* description();
	};

	ToEventlog eventlog;
	ToSyslog syslog;
	ToFile filelog;
	ToStderr stderrlog;

	static const config::DescriptionBase* description();
};

struct NetConfig
{
	unsigned short threads;
	unsigned short maxConnections;

	struct Endpoint
	{
		std::string name;
		std::string address;
		unsigned short port;
		unsigned short maxConnections;

		static const config::DescriptionBase* description();
	};
	std::vector<Endpoint> endpoints;

	struct SSLEndpoint
	{
		std::string name;
		std::string address;
		unsigned short port;
		unsigned short maxConnections;
		std::string key;
		std::string certificate;
		std::string CAdirectory;
		std::string CAchainFile;
		bool verify;

		static const config::DescriptionBase* description();
	};
	std::vector<SSLEndpoint> SSLendpoints;

	static const config::DescriptionBase* description();
};

struct DatabaseConfig
{
	std::string identifier;
	std::string host;
	unsigned short port;
	std::string name;
	std::string user;
	std::string password;
	unsigned short connections;
	unsigned short acquireTimeout;

	static const config::DescriptionBase* description();
};

struct ServiceConfig
{
	std::string name;
	std::string displayName;
	std::string descr;

	static const config::DescriptionBase* description();
};

struct DaemonConfig
{
	std::string user;
	std::string group;
	std::string pidFile;

	static const config::DescriptionBase* description();
};

struct AAAAConfig
{
	struct AuthConfig
	{
		std::string file;
		std::string database;

		static const config::DescriptionBase* description();
	};
	struct AuditConfig
	{
		std::string file;

		static const config::DescriptionBase* description();
	};
};


struct Configuration
{
	ServiceConfig service;
	DaemonConfig daemon;
	NetConfig net;
	LoggerConfig logger;
	std::vector<DatabaseConfig> database;

	static const config::DescriptionBase* description();
};


const config::DescriptionBase* LoggerConfig::ToStderr::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToStderr>
	{
		ThisDescription()
		{
			(*this)
			( "loglevel",		&LoggerConfig::ToStderr::loglevel);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToFile::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToFile>
	{
		ThisDescription()
		{
			(*this)
			( "loglevel",		&LoggerConfig::ToFile::loglevel)
			( "filename",		&LoggerConfig::ToFile::filename);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToSyslog::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToSyslog>
	{
		ThisDescription()
		{
			(*this)
			( "loglevel",		&LoggerConfig::ToSyslog::loglevel)
			( "facility",		&LoggerConfig::ToSyslog::facility)
			( "ident",		&LoggerConfig::ToSyslog::ident);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::ToEventlog::description()
{
	struct ThisDescription :public config::Description<LoggerConfig::ToEventlog>
	{
		ThisDescription()
		{
			(*this)
			( "loglevel",		&LoggerConfig::ToEventlog::loglevel)
			( "name",		&LoggerConfig::ToEventlog::name)
			( "source",		&LoggerConfig::ToEventlog::source);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* LoggerConfig::description()
{
	struct ThisDescription :public config::Description<LoggerConfig>
	{
		ThisDescription()
		{
			(*this)
			( "eventlog",		&LoggerConfig::eventlog)
			( "syslog",		&LoggerConfig::syslog)
			( "filelog",		&LoggerConfig::filelog)
			( "stderrlog",		&LoggerConfig::stderrlog);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::description()
{
	struct ThisDescription :public config::Description<NetConfig>
	{
		ThisDescription()
		{
			(*this)
			( "threads",		&NetConfig::threads)
			( "maxConnections",	&NetConfig::maxConnections)
			( "endpoint",		&NetConfig::endpoints)
			( "SSLendpoint",	&NetConfig::SSLendpoints);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::Endpoint::description()
{
	struct ThisDescription :public config::Description<NetConfig::Endpoint>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&NetConfig::Endpoint::name)
			( "address",		&NetConfig::Endpoint::address)
			( "port",		&NetConfig::Endpoint::port)
			( "maxConnections",	&NetConfig::Endpoint::maxConnections);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* NetConfig::SSLEndpoint::description()
{
	struct ThisDescription :public config::Description<NetConfig::SSLEndpoint>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&NetConfig::SSLEndpoint::name)
			( "address",		&NetConfig::SSLEndpoint::address)
			( "port",		&NetConfig::SSLEndpoint::port)
			( "maxConnections",	&NetConfig::SSLEndpoint::maxConnections)
			( "key",		&NetConfig::SSLEndpoint::key)
			( "certificate",	&NetConfig::SSLEndpoint::certificate)
			( "CAdirectory",	&NetConfig::SSLEndpoint::CAdirectory)
			( "CAchainFile",	&NetConfig::SSLEndpoint::CAchainFile)
			( "verify",		&NetConfig::SSLEndpoint::verify);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DatabaseConfig::description()
{
	struct ThisDescription :public config::Description<DatabaseConfig>
	{
		ThisDescription()
		{
			(*this)
			( "identifier",		&DatabaseConfig::identifier)
			( "host",		&DatabaseConfig::host)
			( "port",		&DatabaseConfig::port)
			( "name",		&DatabaseConfig::name)
			( "user",		&DatabaseConfig::user)
			( "password",		&DatabaseConfig::password)
			( "connections",	&DatabaseConfig::connections)
			( "acquireTimeout",	&DatabaseConfig::acquireTimeout);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* ServiceConfig::description()
{
	struct ThisDescription :public config::Description<ServiceConfig>
	{
		ThisDescription()
		{
			(*this)
			( "name",		&ServiceConfig::name)
			( "displayName",	&ServiceConfig::displayName)
			( "description",	&ServiceConfig::descr);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* DaemonConfig::description()
{
	struct ThisDescription :public config::Description<DaemonConfig>
	{
		ThisDescription()
		{
			(*this)
			( "user",		&DaemonConfig::user)
			( "group",		&DaemonConfig::group)
			( "pidFile",		&DaemonConfig::pidFile);
		}
	};
	static const ThisDescription rt;
	return &rt;
}

const config::DescriptionBase* Configuration::description()
{
	struct ThisDescription :public config::Description<Configuration>
	{
		ThisDescription()
		{
			(*this)
			( "service",		&Configuration::service)
			( "daemon",		&Configuration::daemon)
			( "logger",		&Configuration::logger)
			( "net",		&Configuration::net);
		}
	};
	static const ThisDescription rt;
	return &rt;
}


int main( int, const char**)
{
	Configuration cfg;
	std::string filename( "../src/wolframe.conf");

	std::string configfile = resolvePath( boost::filesystem::absolute( filename).string());
	if ( !boost::filesystem::exists( configfile))	{
		LOG_ERROR << "Configuration file " << configfile << " does not exist.";
		return false;
	}

	std::string errmsg;
	boost::property_tree::ptree pt;
	try
	{
		boost::property_tree::read_info( configfile, pt);
		if (cfg.description()->parse( &cfg, pt, errmsg))
		{
			cfg.description()->print( std::cout, "wolframe", &cfg);
		}
	}
	catch (std::exception& e)
	{
		errmsg = e.what();
	}
	if (!errmsg.empty())
	{
		LOG_ERROR << "Error in configuration: " << errmsg;
	}
	return 0;
}



