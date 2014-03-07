/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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

#include "config/configurationTree.hpp"
#include <string>
#include <ostream>
#include <list>

// forward definition
namespace _Wolframe { namespace module {
	class ModulesDirectory;
}} // namespace _Wolframe::module

namespace _Wolframe {
namespace config {


/// Base class for the configuration structures
class ConfigurationBase
{
public:
	/// Class constructor.
	///\param[in]	name	the name that will be displayed for this
	///			configuration section in messages (log, print ...)
	///			It has no other processing purpose
	///\param[in]	logParent the logging prefix of the parent.
	///\param[in]	logName	the logging name of this section. Combined with
	///			the logParent parameter will form the whole logging
	///			prefix for of the section.
	ConfigurationBase( const char* name, const char* logParent, const char* logName )
	{
		m_sectionName = name ? name : "";
		m_logPrefix = logParent ? logParent : "";
		if ( logName && *logName != '\0' )	{
			m_logPrefix += logName; m_logPrefix += ": ";
		}
	}

	virtual ~ConfigurationBase()			{}

	///\brief The display string (name) for the configuration section
	///\return	a reference to the name set by the constructor
	const std::string& sectionName() const		{ return m_sectionName; }

	/// The prefix for logging messages for this configuration section
	///\return	a reference to the prefix set by the constructor
	const std::string& logPrefix() const		{ return m_logPrefix; }

	///\brief Parse the configuration section
	///\param[in]	pt		property tree node
	///\param[in]	node		the label of the node. It should be
	///				the same (case insensitive) as it->first
	virtual bool parse( const ConfigurationNode& cfgTree, const std::string& node,
			    const module::ModulesDirectory* modules ) = 0;

	/// Set the pathes in the configuration to absolute values
	///\param[in]	refPath	use this path as reference when computing
	///			the absolute pathes
	virtual void setCanonicalPathes( const std::string& /* refPath */ )	{}

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
	///\param[in]	os	stream to use for printing
	///\param[in]	indent	print indented with this number of spaces
	virtual void print( std::ostream& os, size_t indent = 0 ) const = 0;

private:
	std::string			m_sectionName;
	std::string			m_logPrefix;
};


/// A named configuration is a normal configuration that provides
/// also an className function.
class NamedConfiguration : public ConfigurationBase
{
public:
	/// Class constructor.
	NamedConfiguration( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName )	{}

	virtual ~NamedConfiguration()				{}

	virtual const char* className() const = 0;
};

}} // namespace _Wolframe::config

#endif // _CONFIGURATION_BASE_HPP_INCLUDED
