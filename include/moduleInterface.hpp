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
///\file moduleInterface.hpp
//

#ifndef _MODULE_CONFIGURATION_HPP_INCLUDED
#define _MODULE_CONFIGURATION_HPP_INCLUDED

#include <string>
#include <boost/property_tree/ptree.hpp>
#include "config/configurationBase.hpp"

namespace _Wolframe {
namespace module {

class ModuleConfiguration : public config::ConfigurationBase
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
	ModuleConfiguration( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName )	{}

	virtual ~ModuleConfiguration()				{}

	virtual const char* typeName() const = 0;
};

struct ModuleConfigConstructorDescript
{
	const char* typeName;
	const char* sectionTitle;
	const char* sectionName;
	ModuleConfiguration* (*createFunc)( const char* name, const char* logParent, const char* logName );
	bool (*parseFunc)( config::ConfigurationBase&,
			   const boost::property_tree::ptree&, const std::string& );
public:
	ModuleConfigConstructorDescript( const char* tn, const char* st, const char* sn,
					 bool (*pf)( config::ConfigurationBase& configuration,
						     const boost::property_tree::ptree& pt,
						     const std::string& node ),
					 ModuleConfiguration* (*cf)( const char* name,
								     const char* logParent,
								     const char* logName ) )
		: typeName( tn ), sectionTitle( st ), sectionName( sn ),
		  createFunc( cf ), parseFunc( pf )	{}
};


class ModuleContainer
{
public:
	virtual ~ModuleContainer()			{}
	virtual const char* typeName() const = 0;
};

struct ModuleDescription
{
	const char* name;
	ModuleContainer* ( *createFunc )( const ModuleConfiguration& conf );
public:
	ModuleDescription( const char* n, ModuleContainer* ( *f )( const ModuleConfiguration& conf ) )
		: name( n ), createFunc( f )	{}
};

}} // namespace _Wolframe::module

#endif // _MODULE_CONFIGURATION_HPP_INCLUDED
