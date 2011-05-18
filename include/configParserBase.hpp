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
// configuration parser base class
//

#ifndef _CONFIG_PARSERBASE_HPP_INCLUDED
#define _CONFIG_PARSERBASE_HPP_INCLUDED

#include <string>
#include <ostream>
#include <iostream>

#include "configurationBase.hpp"

#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace config	{

class ConfigurationParserBase	{
public:
	/// Class constructor.
	///\param[in]	name	the name that will be displayed for this
	///			configuration section in messages (log, print ...)
	///			It has no other processing purpose
	///\param[in]	logParent the logging prefix of the parent.
	///\param[in]	logName	the logging name of this section. Combined with
	///			the logParent parameter will form the whole logging
	///			prefix for of the section.
	ConfigurationParserBase( const char* name, const char* logParent, const char* logName )
	{
		sectionName_ = name ? name : "";
		logPrefix_ = logParent ? logParent : "";
		if ( logName && *logName != '\0' )	{
			logPrefix_ += logName; logPrefix_ += ": ";
		}
	}

	virtual ~ConfigurationParserBase()	{}

	/// The display string (name) of the configuration section
	///\return	a reference to the name set by the constructor
	const std::string& sectionName() const		{ return sectionName_; }

	/// The prefix for logging messages for this configuration section
	///\return	a reference to the prefix set by the constructor
	const std::string& logPrefix() const		{ return logPrefix_; }

	/// Parse the configuration section (virtual function)
	/// This function must be implemented
	///\param[in]	it		iterator to the property tree node
	///\param[in]	nodeName	the label of the node. It should be
	///				the same (case insensitive) as it->first
	virtual bool parse( const boost::property_tree::ptree& pt,
			    const std::string& nodeName ) = 0;

private:
	std::string	sectionName_;
	std::string	logPrefix_;
};

}} // namespace _Wolframe::config

#endif // _CONFIG_PARSERBASE_HPP_INCLUDED
