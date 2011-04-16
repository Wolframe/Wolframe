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
// AAAA provider implementation
//

#include "AAAAprovider.hpp"
#include "configurationBase.hpp"

namespace _Wolframe {
namespace AAAA {

Configuration::Configuration() : _Wolframe::config::ConfigurationBase( "AAAA Provider" )
{
}

/// methods
bool Configuration::parse( const boost::property_tree::ptree& /* pt */, const std::string& /* node */ )
{
	return true;
}

bool Configuration::check() const
{
	return true;
}

void Configuration::print( std::ostream& os ) const
{
	os << displayName() << " configuration :)";
}

void Configuration::setCanonicalPathes( const std::string& /* referencePath */ )
{
}


AAAAprovider::AAAAprovider( const Configuration& /* conf */ )
	: authenticator_(),
	  authorizer_(),
	  auditor_()
{
}

}} // namespace _Wolframe::AAAA

