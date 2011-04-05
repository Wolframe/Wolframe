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

#ifndef _CONFIG_HELPERS_HPP_INCLUDED
#define _CONFIG_HELPERS_HPP_INCLUDED

#include <string>
#include <ostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>

#include "logger.hpp"

namespace _Wolframe {
namespace Configuration	{

/// Get a boolean value from the property tree
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[out]	value	the (boolean) value read
///\return	true if it succeeds or false otherwise and logs the error
bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& name,
		   const std::string& module, bool& value );


/// Get a boolean value from the property tree
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[out]	value	the (boolean) value read
///\param[in,out] valueIsSet	flag to signal if the value is set
///			if set, the function will log an error and fail
///\return	true if it succeeds or false otherwise and logs the error
bool getBoolValue( const boost::property_tree::ptree& pt, const std::string& name,
		   const std::string& module, bool& value, bool& valueIsSet );


/// Get a string value from the property tree. If the input string is
/// not empty, it will be considered set and the function will log
/// an error and fail.
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[in,out] value	the string read
///\return	true if it succeeds or false otherwise and logs the error
bool getStringValue( const boost::property_tree::ptree& pt, const std::string& name,
		     const std::string& module, std::string& value );


/// Similar to getStringValue but if the value read is "*" it will be
/// replaced with "0.0.0.0"
bool getHostnameValue( const boost::property_tree::ptree& pt, const std::string& name,
		       const std::string& module, std::string& value );


/// Get a non zero intger value. A value of 0 is considered uninitialized.
/// A non-zero value when calling the function will cause the function to fail.
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[in,out] value	the integer value read
///\return	true if it succeeds or false otherwise and logs the error
template <typename T>
bool getNonZeroIntValue( const boost::property_tree::ptree& pt, const std::string& name,
			 const std::string& module, T& value )
{
	if ( value != 0 )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}

	std::string s = pt.get_value<std::string>();
	try	{
		value = boost::lexical_cast<T>( s );
	}
	catch( boost::bad_lexical_cast& e )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \"" << s << "\"";
		return false;
	}

	if ( value == 0 )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \"" << s << "\"";
		return false;
	}
	return true;
}


/// Get a number from a property tree node.
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[out]	value	the integer value read
///\return	true if it succeeds or false otherwise and logs the error
template <typename T>
bool getNumberValue( const boost::property_tree::ptree& pt, const std::string& name,
		     const std::string& module, T& value )
{
	std::string s = pt.get_value<std::string>();
	try	{
		value = boost::lexical_cast<T>( s );
	}
	catch( boost::bad_lexical_cast& e )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \"" << s << "\"";
		return false;
	}
	return true;
}


/// Get a number from a property tree node, check and set valueIsSet flag.
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[out]	value	the integer value read
///\param[in,out] valueIsSet	flag to signal if the value is set
///\return	true if it succeeds or false otherwise and logs the error
template <typename T>
bool getNumberValue( const boost::property_tree::ptree& pt, const std::string& name,
		     const std::string& module, T& value, bool& valueIsSet )
{
	if ( valueIsSet )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}
	std::string s = pt.get_value<std::string>();
	try	{
		value = boost::lexical_cast<T>( s );
	}
	catch( boost::bad_lexical_cast& e )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \"" << s << "\"";
		return false;
	}
	valueIsSet = true;
	return true;
}


/// Get a number from a property tree node, check and set valueIsSet flag.
/// Check low and high limits.
///\param[in]	it	property tree iterator pointing to the node
///\param[in]	module	reference to the module name. Used only for
///			error logging
///\param[out]	value	the integer value read
///\param[in,out] valueIsSet	flag to signal if the value is set
///\param[in]	lowerLimit	the lowest acceptable value
///\param[in]	upperLimit	the highest acceptable value
///\return	true if it succeeds or false otherwise and logs the error
template <typename T>
bool getNumberValue( const boost::property_tree::ptree& pt, const std::string& name,
		     const std::string& module, T& value, bool& valueIsSet,
		     T lowerLimit, T upperLimit )
{
	if ( valueIsSet )	{
		LOG_ERROR << module << ": " << name << " redefined";
		return false;
	}
	std::string s = pt.get_value<std::string>();
	try	{
		value = boost::lexical_cast<T>( s );
	}
	catch( boost::bad_lexical_cast& e )	{
		LOG_ERROR << module << ": invalid value for " << name << ": \"" << s << "\"";
		return false;
	}
	if ( value < lowerLimit )	{
		LOG_ERROR << module << ": invalid value (too low) for " << name << ": \""
			  << pt.get_value<std::string>() << "\"";
		return false;
	}
	if ( value > upperLimit )	{
		LOG_ERROR << module << ": invalid value (too high) for " << name << ": \""
			  << pt.get_value<std::string>() << "\"";
		return false;
	}
	return true;
}

} // namespace Configuration
} // namespace _Wolframe


#endif // _CONFIG_HELPERS_HPP_INCLUDED
