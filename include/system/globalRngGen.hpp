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
//
// Sort of a pseudo-singleton
//

#ifndef _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED
#define _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED

#include <string>

#if defined( _MSC_VER )
	#define WOLFRAME_EXPORT __declspec( dllexport )
#else
	#define WOLFRAME_EXPORT
#endif

namespace _Wolframe	{

class WOLFRAME_EXPORT RandomGenerator
{
public:
	~RandomGenerator();

	static RandomGenerator& instance();
	static RandomGenerator& instance( const std::string &rndDev );

	void device( const std::string &rndDev ){ m_device = rndDev; }
	const std::string& device() const	{ return m_device; }
	unsigned random() const;
	void generate( unsigned char* buffer, size_t bytes ) const;

protected:
	RandomGenerator();
	RandomGenerator( const std::string &rndDev );

private:
	// make it noncopyable
	RandomGenerator( const RandomGenerator& );
	const RandomGenerator& operator= ( const RandomGenerator& );

	// Real object data
	std::string	m_device;		///< random generator device
};

} // namespace _Wolframe

#endif // _GLOBAL_RANDOM_GENERATOR_HPP_INCLUDED
