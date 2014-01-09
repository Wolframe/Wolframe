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
//\file types/syncObjectList.hpp
//\brief Interface to shared and synchronized list of objects
#ifndef _Wolframe_SYNC_OBJECT_LIST_HPP_INCLUDED
#define _Wolframe_SYNC_OBJECT_LIST_HPP_INCLUDED
#include "types/syncCounter.hpp"
#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace _Wolframe {
namespace types {

//\class SyncObjectList
//\brief Shared and synchronized list of objects
template <class OBJ>
class SyncObjectList
{
public:
	typedef typename std::list<OBJ>::iterator ElementReference;
	
	//\brief Constructor
	SyncObjectList()
	{}
	//\brief Destructor
	virtual ~SyncObjectList(){}

	//\brief Insert object into the list
	//\param[in] obj object to insert by value
	//\param[out] ref handle to the object returned
	//\return true, if success, false if constraint failed
	virtual bool insert( const OBJ& obj, ElementReference& ref)
	{
		boost::mutex::scoped_lock lock( m_mutex);
		m_list.push_front( obj);
		ref = m_list.begin();
		return true;
	}

	virtual void release( const ElementReference& objref)
	{
		boost::mutex::scoped_lock lock( m_mutex);
		m_list.erase( objref);
	}

private:
	boost::mutex m_mutex;		//< mutex for mutual exclusion of writes
	std::list<OBJ> m_list;		//< list of objects
};


//\class BoundedSyncObjectList
//\brief SyncObjectList with limit for maximum number of elements to insert
template <class OBJ>
class BoundedSyncObjectList
	:public SyncObjectList<OBJ>
{
public:
	typedef SyncObjectList<OBJ> Parent;
	typedef typename std::list<OBJ>::iterator ElementReference;

	//\brief Constructor
	explicit BoundedSyncObjectList( unsigned int maxNofObjects=SyncCounter::NoLimit, SyncCounter* globalCounter_=0)
		:m_counter(maxNofObjects)
		,m_globalCounter(globalCounter_){}

	//\brief Destructor
	virtual ~BoundedSyncObjectList(){}

	//\brief Insert object into the list
	//\param[in] obj object to insert by value
	//\param[out] ref handle to the object returned
	//\return true, if success, false if the maxinum number of elements limit has been reached
	virtual bool insert( const OBJ& obj, ElementReference& ref)
	{
		if (m_globalCounter)
		{
			types::SyncCounter::ScopedAquire gs( *m_globalCounter);
			{
				if (!gs.entered()) return false;
	
				types::SyncCounter::ScopedAquire ls( m_counter);
				{
					if (!ls.entered()) return false;
					if (!Parent::insert( obj, ref)) return false;
					ls.done();
				}
				gs.done();
			}
			return true;
		}
		else
		{
			types::SyncCounter::ScopedAquire ls( m_counter);
			{
				if (!ls.entered()) return false;
				if (!Parent::insert( obj, ref)) return false;
				ls.done();
			}
			return true;
		}
	}

	//\brief Remove object referenced from list
	//\param[in] obj object reference (iterator) in list
	virtual void release( const ElementReference& objref)
	{
		m_counter.release();
		if (m_globalCounter) m_globalCounter->release();
		Parent::release( objref);
	}

private:
	SyncCounter m_counter;		//< counter for maximum number of elements limit
	SyncCounter* m_globalCounter;	//< global counter for maximum total number of elements limit
};

}}//namespace
#endif


