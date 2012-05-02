#include "gtest/gtest.h"

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
	memset( ft, NOF_ITERS, sizeof( FILETIME ) );
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
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}

#if 0

int main( int argc, char *argv[] )
{
	// not quite accurate, Windows, roughly 10 ms resolution,
	// Unix no problem, down to the microsecond different
#ifdef _WIN32
	for( int i = 0; i < nof_iters; i++ ) {
		get_time(frequency, ref_point, file_time[i]);
	}
	for( int i = 0; i < nof_iters; i++ ) {
		::FileTimeToSystemTime(&file_time[i], &system_time[i]);
	}		
	tcount = 0;
	SYSTEMTIME slast = system_time[0];
	FILETIME flast = file_time[0];
	std::cout << "Win32 article simplistic sync" << std::endl;
	for( int i = 0; i < nof_iters; i++ ) {
		if( flast.dwLowDateTime != file_time[i].dwLowDateTime ) {
//		if( slast.wMilliseconds != system_time[i].wMilliseconds ) {
			std::cout 	<< tcount + 1 << " times "
					<< file_time[i-1].dwHighDateTime << "."
					<< file_time[i-1].dwLowDateTime
/*
					<< std::setw(2) << system_time[i-1].wHour << ':'
					<< std::setw(2) << system_time[i-1].wMinute << ':'
					<< std::setw(2) << system_time[i-1].wSecond << '.'
					<< std::setw(3) << system_time[i-1].wMilliseconds
*/
					<< std::endl;
			tcount = 0;
//			slast = system_time[i];
			flast = file_time[i];
		} else {
			tcount++;
		}
	}	
	if( tcount > 0 ) {
		std::cout 	<< tcount << " times "
				<< file_time[nof_iters-1].dwHighDateTime << "."
				<< file_time[nof_iters-1].dwLowDateTime
//				<< std::setw(2) << system_time[nof_iters-1].wHour << ':'
//				<< std::setw(2) << system_time[nof_iters-1].wMinute << ':'
//				<< std::setw(2) << system_time[nof_iters-1].wSecond << '.'
//				<< std::setw(3) << system_time[nof_iters-1].wMilliseconds
				<< std::endl;
	}
	std::cout << "------------------" << std::endl;	
#endif

	return 0;
}
#endif
