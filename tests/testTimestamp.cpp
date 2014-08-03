#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

#define NOF_ITERS 10

TEST( Timestamp, Precision )
{
#ifndef _WIN32
	boost::posix_time::ptime t[NOF_ITERS];
	
	for( int i = 0; i < NOF_ITERS; i++ ) {
		t[i] = boost::posix_time::ptime(
			boost::posix_time::microsec_clock::universal_time( ) );
	}
	
	for( int i = 0; i < NOF_ITERS; i++ ) {
		std::cout << t[i] << std::endl;
	}
#endif

#ifdef _WIN32
	FILETIME ft[NOF_ITERS];
	memset( ft, 0, NOF_ITERS * sizeof( FILETIME ) );
	LARGE_INTEGER li;

	for( int i = 0; i < NOF_ITERS; i++ ) {
		GetSystemTimeAsFileTime( &ft[i] );
		QueryPerformanceCounter( &li );
		ft[i].dwLowDateTime += li.QuadPart;
	}
	
	for( int i = 0; i < NOF_ITERS; i++ ) {
		std::cout << ft[i].dwHighDateTime << "." << ft[i].dwLowDateTime << std::endl;
	}
#endif

	SUCCEED( );
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
