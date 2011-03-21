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
