#ifndef _CONFIG_HELPERS_HPP_INCLUDED
#define _CONFIG_HELPERS_HPP_INCLUDED

#include <string>
#include <ostream>

#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
	namespace Configuration	{

		bool getBoolValue( const boost::property_tree::ptree::const_iterator it,
				   const std::string& module, bool& value );

		bool getBoolValue( const boost::property_tree::ptree::const_iterator it,
				   const std::string& module, bool& value, bool& valueIsSet );


		bool getStringValue( const boost::property_tree::ptree::const_iterator it,
				     const std::string& module, std::string& value );

		bool getHostnameValue( const boost::property_tree::ptree::const_iterator it,
				       const std::string& module, std::string& value );

		template <typename T>
		bool getNonZeroIntValue( const boost::property_tree::ptree::const_iterator it,
					 const std::string& module, T& value );

		template <typename T>
		bool getIntegerValue( const boost::property_tree::ptree::const_iterator it,
				      const std::string& module, T& value );

		template <typename T>
		bool getIntegerValue( const boost::property_tree::ptree::const_iterator it,
				      const std::string& module, T& value, bool& valueIsSet );

		template <typename T>
		bool getIntegerValue( const boost::property_tree::ptree::const_iterator it,
				      const std::string& module, T& value, bool& valueIsSet,
				      T lowerLimit, T upperLimit );

	} // namespace Configuration
} // namespace _Wolframe


#endif // _CONFIG_HELPERS_HPP_INCLUDED
