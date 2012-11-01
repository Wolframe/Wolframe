/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// Sort of a pseudo-singleton
//

#ifndef _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED
#define _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED

#include <string>
#include <stdexcept>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

namespace _Wolframe	{

class GlobalRandomGenerator : public boost::noncopyable
{
public:
	GlobalRandomGenerator( std::string& rndDev );

	static GlobalRandomGenerator& instance()
	{
		if ( !m_initialized )
			throw std::runtime_error( "Global random number generator not instantiated yet" );
		return *t;
	}

//	const std::string& device() const	{ return m_device; }
	unsigned random();
	int random( unsigned char* buffer, size_t bytes );

private:
	std::string	m_device;		///< random generator device
	static bool	m_initialized;
	static boost::scoped_ptr< GlobalRandomGenerator >	t;
};
boost::scoped_ptr< GlobalRandomGenerator > GlobalRandomGenerator::t( 0 );
bool GlobalRandomGenerator::m_initialized = false;

} // namespace _Wolframe

#endif // _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED
