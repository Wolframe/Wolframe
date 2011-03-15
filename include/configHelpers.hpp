#ifndef _CONFIG_HELPERS_HPP_INCLUDED
#define _CONFIG_HELPERS_HPP_INCLUDED

#include <string>
#include <ostream>

#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
	namespace Configuration	{

		bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& module,
				   const std::string& name, bool& value );

		bool getStringValue( const boost::property_tree::ptree& pt, const std::string& module,
				     const std::string& name, std::string& value );

		bool getHostnameValue( const boost::property_tree::ptree& pt, const std::string& module,
				       const std::string& name, std::string& value );

		bool getUnsignedShortValue( const boost::property_tree::ptree& pt, const std::string& module,
					    const std::string& name, unsigned short& value );

	} // namespace Configuration
} // namespace _Wolframe


#endif // _CONFIG_HELPERS_HPP_INCLUDED
