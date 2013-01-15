/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
// singleton.hpp
//

#ifndef _SINGLETON_HPP_INCLUDED
#define _SINGLETON_HPP_INCLUDED

#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

// Warning: If T's constructor throws, instance() will return a null reference.

// the initialized flag might not be needed but it looks like it increases performance

template<class T>
class Singleton : private boost::noncopyable
{
public:
	static T& instance()
	{
		if ( !initialized )
			boost::call_once( init, flag );
		return *t;
	}

protected:
	Singleton()	{}
	~Singleton()	{}

	static void init() // never throws
	{
		t.reset( new T() );
		initialized = true;
	}

private:
	static boost::scoped_ptr<T>	t;
	static boost::once_flag		flag;
	static bool			initialized;
};


template<class T> boost::scoped_ptr<T> Singleton<T>::t(0);
template<class T> boost::once_flag Singleton<T>::flag = BOOST_ONCE_INIT;
template<class T> bool Singleton<T>::initialized = false;

#endif	// _SINGLETON_HPP_INCLUDED
