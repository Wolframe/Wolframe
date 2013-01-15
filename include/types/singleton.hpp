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
#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

// Warning: If T's constructor throws, instance() will return a null reference.

template< class T >
class Singleton : private boost::noncopyable
{
public:
	static T& instance()
	{
		if ( !m_initialized )	{
			boost::lock_guard<boost::mutex> lock( m_mutex );
			if ( !m_initialized )	{
				m_t.reset( new T() );
				m_initialized = true;
			}
		}
		return *m_t;
	}

	template< typename ArgType >
	static T& instance( ArgType arg )
	{
		if ( !m_initialized )	{
			boost::lock_guard<boost::mutex> lock( m_mutex );
			if ( !m_initialized )	{
				m_t.reset( new T( arg ) );
				m_initialized = true;
			}
		}
		return *m_t;
	}

	template< typename ArgType0, typename ArgType1 >
	static T& instance( ArgType0 arg0, ArgType1 arg1 )
	{
		if ( !m_initialized )	{
			boost::lock_guard<boost::mutex> lock( m_mutex );
			if ( !m_initialized )	{
				m_t.reset( new T( arg0, arg1 ) );
				m_initialized = true;
			}
		}
		return *m_t;
	}

protected:
	Singleton()	{}
	~Singleton()	{}

private:
	static boost::scoped_ptr<T>	m_t;
	static boost::mutex		m_mutex;
	static bool			m_initialized;
};

template<class T> boost::scoped_ptr<T> Singleton<T>::m_t( 0 );
template<class T> boost::mutex Singleton<T>::m_mutex;
template<class T> bool Singleton<T>::m_initialized = false;

#endif	// _SINGLETON_HPP_INCLUDED
