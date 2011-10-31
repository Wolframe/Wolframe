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
// reference.hpp
//

#ifndef _REFERENCE_CONFIG_HPP_INCLUDED
#define _REFERENCE_CONFIG_HPP_INCLUDED

#include "config/configurationBase.hpp"

namespace _Wolframe {
namespace config {

/// reference (label) class
/// note that this is a configuration class only
class ReferenceConfig : public ConfigurationBase
{
	friend class ConfigurationParser;
public:
	ReferenceConfig( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName )	{}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;

	const std::string& label() const		{ return m_ref; }
private:
	std::string	m_ref;
};

}} // namespace _Wolframe::config

#endif // _REFERENCE_CONFIG_HPP_INCLUDED
