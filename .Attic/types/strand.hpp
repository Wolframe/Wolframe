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
///\file types/strand.hpp
///\brief Strand implementation (alternative to boost asio strand)

#ifndef _Wolframe_TYPES_STRAND_HPP_INCLUDED
#define _Wolframe_TYPES_STRAND_HPP_INCLUDED
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

template <class Object, typename Error>
class Strand
{
public:
	Strand()
		:m_executing(false){}

	struct Call
	{
		boost::shared_ptr<Object> objref;
		typedef void (Object::*Method)( const Error&, std::size_t);
		Method method;
		Error error;
		std::size_t size;

		Call()
			:size(0){}
		Call( const boost::shared_ptr<Object>& objref_, Method method_, Error error_, std::size_t size_=0)
			:objref(objref_),method(method_),error(error_),size(size_){}
		Call( const boost::shared_ptr<Object>& objref_, Method method_)
			:objref(objref_),method(method_),size(0){}
		Call( const Call& o)
			:objref(o.objref),method(o.method),error(o.error),size(o.size){}
	};

	void run()
	{
		Call call;
		for (;;)
		{
			{
				boost::mutex::scoped_lock lock( m_mutex );
				call = m_queue.front();
			}
			Object* ptr = call.objref.get();
			(ptr->*call.method)( call.error, call.size);
			{
				boost::mutex::scoped_lock lock( m_mutex );
				m_queue.pop_front();
				if (m_queue.empty())
				{
					m_executing = false;
					return;
				}
			}
		}
	}

	void invoke( const Call& call)
	{
		bool mine = false;
		{
			boost::mutex::scoped_lock lock( m_mutex );
			m_queue.push_back( call);
			if (!m_executing)
			{
				mine = m_executing = true;
			}
		}
		if (mine) run();
	}

private:
	boost::mutex m_mutex;
	std::list<Call> m_queue;
	bool m_executing;
};

}}//namespace
#endif

