#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

int main( )
{
	boost::posix_time::ptime t;

	for( int i = 1; i < 10000; i++ ) {
		t = boost::posix_time::ptime(
			boost::posix_time::microsec_clock::universal_time( ) );
		std::cout << t << std::endl;
	}

	return 0;
}
