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
// objectPool template unit tests
//

#include <cstdlib>
#include <cassert>
#include <boost/thread/thread.hpp>

#include "objectPool.hpp"
#include "gtest/gtest.h"

class testObject	{
public:
	testObject()		{ m_threads = 0, m_uses = 0; }
	void doSomething()	{
		assert( m_threads == 0 );
		m_threads++, m_uses++;
		assert( m_threads == 1 );
		m_threads--;
		assert( m_threads == 0 );
	}
	void sleepSomething()	{
		assert( m_threads == 0 );
		m_threads++, m_uses++;
		assert( m_threads == 1 );
		boost::this_thread::sleep( boost::posix_time::microseconds( rand() % 150 ));
		assert( m_threads == 1 );
		m_threads--;
		assert( m_threads == 0 );
	}
	bool unused()		{ return m_threads == 0; }
	int threads()		{ return m_threads; }
	unsigned long used()	{ return m_uses; }
private:
	int		m_threads;
	unsigned long	m_uses;
};


// The fixture for testing Wolframe::ObjectPool template
class ObjectPoolFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	ObjectPoolFixture()	{
		srand(clock());
		noThreads = 1 + (int)( rand() % 32 );
		poolSize = 1 + (int)( rand() % 16 );
		times = (unsigned long)( rand() % 200000 );
		std::cout << noThreads << " threads, " << poolSize << " elements in the pool, "
			  << times << " iterations, total "<< noThreads * times << std::endl;
	}


	// Clean-up work that doesn't throw exceptions here.
	virtual ~ObjectPoolFixture()	{
	}

	// Code here will be called immediately after the constructor (right
	// before each test).
	virtual void SetUp() {
		for ( unsigned long i = 0; i < poolSize; i++ )	{
			testObject *tstObj = new testObject();
			tstObjs.push_back( tstObj );
			objPool.add( tstObj );
		}
	}

	// Code here will be called immediately after each test (right
	// before the destructor).
	virtual void TearDown() {
		if ( ! emptyPool )
			for ( unsigned long i = 0; i < poolSize; i++ )
				objPool.get();
		for ( unsigned long i = 0; i < poolSize; i++ )
			delete tstObjs[i];
	}

	// Objects declared here can be used by all tests in the test case.
	int					noThreads;
	std::vector< boost::thread* >		threads;
	std::vector< testObject* >		tstObjs;
	_Wolframe::ObjectPool< testObject* >	objPool;
	unsigned				poolSize;
	unsigned long				times;
	bool					emptyPool;

public:
	static void testThread( _Wolframe::ObjectPool< testObject* > *pool, unsigned count )
	{
		for ( std::size_t i = 0; i < count; )	{
			try	{
				_Wolframe::PoolObject< testObject* > tstObj( *pool );
				tstObj->doSomething();
				i++;
			}
			catch ( _Wolframe::ObjectPoolTimeout )
			{
			}
		}
	}

	static void sleepTestThread( _Wolframe::ObjectPool< testObject* > *pool, unsigned count )
	{
		for ( std::size_t i = 0; i < count; )	{
			try	{
				_Wolframe::PoolObject< testObject* > tstObj( *pool );
				tstObj->sleepSomething();
				i++;
			}
			catch ( _Wolframe::ObjectPoolTimeout )
			{
			}
		}
	}

};


// Tests the ObjectPool get & release
TEST_F( ObjectPoolFixture, noTimeout )	{
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_EQ( tstObjs[i]->used(), 0 );
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	}

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::testThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, noTimeoutSleep )	{
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_EQ( tstObjs[i]->used(), 0 );
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	}

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::sleepTestThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, Timeout )	{
	objPool.timeout( 1 );
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_EQ( tstObjs[i]->used(), 0 );
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	}

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::testThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, TimeoutSleep )	{
	objPool.timeout( 1 );
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_EQ( tstObjs[i]->used(), 0 );
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	}

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::sleepTestThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	for ( unsigned long i = 0; i < poolSize; i++ )
		ASSERT_EQ( tstObjs[i]->threads(), 0 );
	ASSERT_EQ( used, noThreads * times );
	emptyPool = false;
}

TEST_F( ObjectPoolFixture, TestTimeout )	{
	objPool.timeout( 1 );

	for ( unsigned long i = 0; i < poolSize; i++ )
		objPool.get();
	ASSERT_THROW( objPool.get(), _Wolframe::ObjectPoolTimeout );
	emptyPool = true;
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
