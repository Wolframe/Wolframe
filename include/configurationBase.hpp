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

#ifndef _CONFIGURATION_BASE_HPP_INCLUDED
#define _CONFIGURATION_BASE_HPP_INCLUDED

#include <string>
#include <ostream>
#include <iostream>

#include <boost/property_tree/ptree.hpp>

//// forward declaration for boost::property_tree
//// we should make this work one day
//namespace boost	{
//	namespace property_tree	{
//		struct	ptree;
//	}
//}

namespace _Wolframe {
	namespace Configuration	{

		struct ConfigurationBase	{
		public:
			/// Class constructor.
			///\param[in]	name	the name that will be displayed for this
			///			configuration section in messages (log, print ...)
			///			It has no processing purpose
			ConfigurationBase( const std::string& name )	{ dispName_ = name; }

			/// The display string (name) of the configuration section
			///\return	a reference to the name set by the constructor
			const std::string& displayName() const		{ return dispName_; }

			/// Parse the configuration section (virtual function)
			/// This function must be implemented
			///\param[in]	it		iterator to the property tree node
			///\param[in]	nodeName	the label of the node. It should be
			///				the same (case insensitive) as it->first
			virtual bool parse( const boost::property_tree::ptree& pt,
					    const std::string& nodeName ) = 0;
//			virtual bool parse( const boost::property_tree::ptree::const_iterator it,
//					   const std::string& nodeName ) = 0;
			/// Set the pathes in the configuration to absolute values
			///\param[in]	referencePath	use this path as reference when
			///				computing the absolute pathes
			virtual void setCanonicalPathes( const std::string& /* referencePath */ )	{}

			/// Check if the server configuration makes sense
			///
			/// Be aware that this function does NOT test if the configuration
			/// can be used. It only tests if it MAY be valid.
			/// This function will log errors / warnings
			///\return	true if the configuration has no errors, false
			///		otherwise
			virtual bool check() const			{ return true; }

			// these functions are not implemented / implementable yet
			// and I am not sure if it should be here or not
			// virtual bool test() const = 0;

			/// Print the configuration
			/// This function is supposed to print the running configuration, this means
			/// all the configuration parameters, not only those that were set in the
			/// configuration file.
			virtual void print( std::ostream& os ) const = 0;
		private:
			std::string	dispName_;
		};

	} // namespace Configuration
} // namespace _Wolframe

#endif // _CONFIGURATION_BASE_HPP_INCLUDED
