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
//\file connectionListTest.hpp
//\brief Tests for connection bookkeeping data structures
#include "connectionType.hpp"
#include <cstring>
#include <cstdlib>
#include <boost/thread/thread.hpp>
#include "gtest/gtest.h"

using namespace _Wolframe;
using namespace _Wolframe::net;

class ConnectionListFixture
	:public ::testing::Test
{
public:
	struct RunStats
	{
		unsigned int nofConnectionsDeleted;
		unsigned int nofConnectionsRefused;
		unsigned int nofConnectionsGranted;
		unsigned int nofGlobalBoundaryLimitErrors;
		unsigned int nofLocalBoundaryLimitErrors;

		RunStats()
			:nofConnectionsDeleted(0)
			,nofConnectionsRefused(0)
			,nofConnectionsGranted(0)
			,nofGlobalBoundaryLimitErrors(0)
			,nofLocalBoundaryLimitErrors(0)
		{}
	};

	struct ThreadData
	{
		enum {NofConn=128};
		RunStats runStats;
		ConnectionType* connar[ NofConn];

		ThreadData()
		{
			unsigned int ii=0;
			for (ii=0; ii<NofConn; ++ii) connar[ii] = 0;
		}

		~ThreadData()
		{
			unsigned int ii=0;
			for (ii=0; ii<NofConn; ++ii)
			{
				if (connar[ii]) delete connar[ii];
			}
		}
	};

	ConnectionListFixture()
		:m_globalCounter( 401)
		,m_connlist1( new ConnectionTypeList( 200, &m_globalCounter))
		,m_connlist2( new ConnectionTypeList( 200, &m_globalCounter))
		,m_nofThreads( 16)
		,m_nofRuns( 100000)
	{
		//... random seed changes only every day (so we can debug errors occurred)
		m_data1 = new ThreadData[ m_nofThreads];
		m_data2 = new ThreadData[ m_nofThreads];
	}

	virtual ~ConnectionListFixture()
	{
		delete [] m_data1;
		delete [] m_data2;
	}

	virtual void SetUp()
	{}

	virtual void TearDown()
	{}

	static void run( unsigned int nofRuns, ThreadData* data, ConnectionTypeList* connlist)
	{
		unsigned int ii;
		unsigned int xx = rand();

		for (ii = 0; ii < nofRuns; ii++)
		{
			unsigned int ai = xx % ThreadData::NofConn;
			xx = rand();
			if (data->connar[ai])
			{
				delete data->connar[ai];
				data->connar[ai] = 0;
				data->runStats.nofConnectionsDeleted++;
			}
			else
			{
				data->connar[ai] = new ConnectionType( connlist);
				if (data->connar[ai]->registerConnection())
				{
					data->runStats.nofConnectionsGranted++;
				}
				else
				{
					switch (connlist->lastError())
					{
						case ConnectionTypeList::Ok:
						case ConnectionTypeList::GlobalBoundaryLimitError:
							data->runStats.nofGlobalBoundaryLimitErrors++;
						break;
						case ConnectionTypeList::LocalBoundaryLimitError:
							data->runStats.nofLocalBoundaryLimitErrors++;
						break;
					}
					data->runStats.nofConnectionsRefused++;
				}
			}
			
		}
	}

	unsigned int countConnections( const ThreadData* data) const
	{
		unsigned int rt = 0;
		unsigned int ti = 0, ii = 0;

		for (ti=0; ti<m_nofThreads; ++ti)
		{
			for (ii=0; ii<ThreadData::NofConn; ++ii)
			{
				if (data[ti].connar[ii] && data[ti].connar[ii]->registered()) ++rt;
			}
		}
		return rt;
	}

	unsigned int countThreadConnections( const ThreadData* data, unsigned int threadidx) const
	{
		unsigned int rt = 0;
		unsigned ii = 0;

		for (ii=0; ii<ThreadData::NofConn; ++ii)
		{
			if (data[threadidx].connar[ii] && data[threadidx].connar[ii]->registered()) ++rt;
		}
		return rt;
	}

	void getTotalStats( RunStats& runStats)
	{
		runStats.nofConnectionsDeleted = 0;
		runStats.nofConnectionsGranted = 0;
		runStats.nofConnectionsRefused = 0;
		runStats.nofGlobalBoundaryLimitErrors = 0;
		runStats.nofLocalBoundaryLimitErrors = 0;

		unsigned int ii = 0;
		for (; ii<m_nofThreads; ++ii)
		{
			runStats.nofConnectionsDeleted += m_data1[ii].runStats.nofConnectionsDeleted;
			runStats.nofConnectionsGranted += m_data1[ii].runStats.nofConnectionsGranted;
			runStats.nofConnectionsRefused += m_data1[ii].runStats.nofConnectionsRefused;
			runStats.nofGlobalBoundaryLimitErrors += m_data1[ii].runStats.nofGlobalBoundaryLimitErrors;
			runStats.nofLocalBoundaryLimitErrors += m_data1[ii].runStats.nofLocalBoundaryLimitErrors;
			runStats.nofConnectionsDeleted += m_data2[ii].runStats.nofConnectionsDeleted;
			runStats.nofConnectionsGranted += m_data2[ii].runStats.nofConnectionsGranted;
			runStats.nofConnectionsRefused += m_data2[ii].runStats.nofConnectionsRefused;
			runStats.nofGlobalBoundaryLimitErrors += m_data2[ii].runStats.nofGlobalBoundaryLimitErrors;
			runStats.nofLocalBoundaryLimitErrors += m_data2[ii].runStats.nofLocalBoundaryLimitErrors;
		}
	}

protected:
	types::SyncCounter m_globalCounter;
	boost::shared_ptr<ConnectionTypeList> m_connlist1;
	boost::shared_ptr<ConnectionTypeList> m_connlist2;
	unsigned int m_nofThreads;
	unsigned int m_nofRuns;
	std::vector<boost::thread*> m_threads;
	ThreadData* m_data1;
	ThreadData* m_data2;
};


TEST_F( ConnectionListFixture, RandomConnectionInserts)
{
	unsigned int ii;
	for (ii = 0; ii < m_nofThreads; ii++)
	{
		boost::thread* thread;
		thread = new boost::thread( &ConnectionListFixture::run, m_nofRuns, m_data1+ii, m_connlist1.get());
		m_threads.push_back( thread);
		thread = new boost::thread( &ConnectionListFixture::run, m_nofRuns, m_data2+ii, m_connlist2.get());
		m_threads.push_back( thread);
	}
	std::vector<boost::thread*>::const_iterator ti = m_threads.begin(), te = m_threads.end();
	for (; ti != te; ti++)
	{
		(*ti)->join();
		delete *ti;
	}
	RunStats runStats;
	getTotalStats( runStats);
	std::cout << "Total number of connections granted: " << runStats.nofConnectionsGranted << std::endl;
	std::cout << "Total number of connections refused: " << runStats.nofConnectionsRefused << std::endl;
	std::cout << "Total number of connections deleted: " << runStats.nofConnectionsDeleted << std::endl;
	unsigned int sum = runStats.nofConnectionsGranted + runStats.nofConnectionsRefused + runStats.nofConnectionsDeleted;
	std::cout << "Total number of iterations: " << sum << std::endl;

	std::cout << "Total number of global boundary limit errors: " << runStats.nofGlobalBoundaryLimitErrors << std::endl;
	std::cout << "Total number of local boundary limit errors: " << runStats.nofLocalBoundaryLimitErrors << std::endl;

	std::cout << "Number of connections alive (1): " << countConnections( m_data1) << std::endl;
	std::cout << "Number of connections alive (2): " << countConnections( m_data2) << std::endl;
	std::cout << "Distribution (1):";
	for (ii = 0; ii < m_nofThreads; ii++)
	{
		std::cout << " " << countThreadConnections( m_data1, ii);
	}
	std::cout << std::endl;
	std::cout << "Distribution (2):";
	for (ii = 0; ii < m_nofThreads; ii++)
	{
		std::cout << " " << countThreadConnections( m_data2, ii);
	}
	std::cout << std::endl;

	ASSERT_TRUE( m_connlist1->check());
	ASSERT_TRUE( m_connlist2->check());

	ASSERT_EQ( countConnections( m_data1), m_connlist1->size());
	ASSERT_EQ( countConnections( m_data2), m_connlist2->size());
}


int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	unsigned int seed = time(0)/(24*60*60);
	std::cout << "Random seed: " << seed << std::endl;
	::srand( seed);
	return RUN_ALL_TESTS();
}


