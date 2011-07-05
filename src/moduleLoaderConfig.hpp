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
// module loader configuration structure
//

#ifndef _MODULE_LOADER_CONFIG_HPP_INCLUDED
#define _MODULE_LOADER_CONFIG_HPP_INCLUDED

#include <list>
#include <string>

#include "config/configurationBase.hpp"

namespace _Wolframe {
namespace config {

/// module loader configuration
class ModuleLoaderConfiguration : public _Wolframe::config::ConfigurationBase
{
	friend class ConfigurationParser;
public:
	/// constructor
	ModuleLoaderConfiguration() : ConfigurationBase( "Module(s) to load", NULL, "Module loader" )	{}
	~ModuleLoaderConfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
#if !defined( _WIN32 )
	void setCanonicalPathes( const std::string& referencePath );
#endif // !defined( _WIN32 )
private:
	std::list< std::string* >	m_moduleFile;
};

}} // namespace _Wolframe::config

#endif // _MODULE_LOADER_CONFIG_HPP_INCLUDED
