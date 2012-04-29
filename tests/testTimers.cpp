#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>
#include <iomanip>
#endif

#ifdef _WIN32
struct reference_point
{
  FILETIME file_time;
  LARGE_INTEGER counter;
};

void simplistic_synchronize(reference_point& ref_point)
{
  FILETIME      ft0 = { 0, 0 },
                ft1 = { 0, 0 };
  LARGE_INTEGER li;

  //
  // Spin waiting for a change in system time. Get the matching
  // performance counter value for that time.
  //
  ::GetSystemTimeAsFileTime(&ft0);
  do
  {
    ::GetSystemTimeAsFileTime(&ft1);
    ::QueryPerformanceCounter(&li);
  }
  while ((ft0.dwHighDateTime == ft1.dwHighDateTime) &&
         (ft0.dwLowDateTime == ft1.dwLowDateTime));

  ref_point.file_time = ft1;
  ref_point.counter = li;
}

void get_time(LARGE_INTEGER frequency, const reference_point& 
    reference, FILETIME& current_time)
{
  LARGE_INTEGER li;

  ::QueryPerformanceCounter(&li);

  //
  // Calculate performance counter ticks elapsed
  //
  LARGE_INTEGER ticks_elapsed;
  
  ticks_elapsed.QuadPart = li.QuadPart - 
      reference.counter.QuadPart;

  //
  // Translate to 100-nanoseconds intervals (FILETIME 
  // resolution) and add to
  // reference FILETIME to get current FILETIME.
  //
  ULARGE_INTEGER filetime_ticks,
                 filetime_ref_as_ul;

  filetime_ticks.QuadPart = 
      (ULONGLONG)((((double)ticks_elapsed.QuadPart/(double)
      frequency.QuadPart)*10000000.0)+0.5);
  filetime_ref_as_ul.HighPart = reference.file_time.dwHighDateTime;
  filetime_ref_as_ul.LowPart = reference.file_time.dwLowDateTime;
  filetime_ref_as_ul.QuadPart += filetime_ticks.QuadPart;

  //
  // Copy to result
  //
  current_time.dwHighDateTime = filetime_ref_as_ul.HighPart;
  current_time.dwLowDateTime = filetime_ref_as_ul.LowPart;
}
#endif

int main( int argc, char *argv[] )
{
	boost::posix_time::ptime t;

	if( argc != 2 ) {
		std::cerr << "usage: testTimers.exe <number of tests>" << std::endl;
		return 1;
	}
	int nof_iters = atoi( argv[1] );
	
	// not quite accurate, Windows, roughly 10 ms resolution,
	// Unix no problem, down to the microsecond different
	std::cout << "boost::posix_time" << std::endl;
	for( int i = 0; i < nof_iters; i++ ) {
		t = boost::posix_time::ptime(
			boost::posix_time::microsec_clock::universal_time( ) );
		std::cout << t << std::endl;
	}
	std::cout << "------------------" << std::endl;

#ifdef _WIN32
	// _ftime_s: much faster, but also ~10 ms resolution
	struct _timeb tb;
	
	std::cout << "Win32 _ftime_s" << std::endl;
	for( int i = 0; i < nof_iters; i++ ) {
		_ftime_s( &tb );
		std::cout 	<< std::setfill( '0' )
				<< tb.time << "." << std::setw( 3 )
				<< tb.millitm << std::endl;
	}
	std::cout << "------------------" << std::endl;
	
	// GetSystemTime (also ~10 ms resolution)
	SYSTEMTIME st;
	std::cout << "Win32 GetSystemTime" << std::endl;
	for( int i = 0; i < nof_iters; i++ ) {
		GetSystemTime( &st );
		std::cout 	<< std::setfill( '0' )
				<< std::setw( 2 ) << st.wHour << ':'
				<< std::setw( 2 ) << st.wMinute << ':'
				<< std::setw( 2 ) << st.wSecond << '.'
				<< std::setw( 3 ) << st.wMilliseconds << std::endl;
	}
	std::cout << "------------------" << std::endl;	
	
	// GetSystemTimeAsFileTime: the same
	// GetTickCount has the same resolution (by documentation)
	
	
	// from http://msdn.microsoft.com/en-us/magazine/cc163996.aspx,
	// simple example
	reference_point ref_point;
	LARGE_INTEGER   frequency;
	FILETIME        file_time;
	SYSTEMTIME      system_time;

	std::cout << "Win32 article simplistic sync" << std::endl;
	::QueryPerformanceFrequency(&frequency);
	simplistic_synchronize(ref_point);
	for( int i = 0; i < nof_iters; i++ ) {
		get_time(frequency, ref_point, file_time);
		::FileTimeToSystemTime(&file_time, &system_time);
		std::cout 	<< std::setw(2) << system_time.wHour << ':'
				<< std::setw(2) << system_time.wMinute << ':'
				<< std::setw(2) << system_time.wSecond << '.'
				<< std::setw(3) << system_time.wMilliseconds << std::endl;
	}	
	std::cout << "------------------" << std::endl;	
#endif

	return 0;
}
