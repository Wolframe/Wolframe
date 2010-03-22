//
// version class tests

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "version class test"

#include <boost/test/unit_test.hpp>


#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include "version.hpp"


#define	MAX_STRING	128


BOOST_AUTO_TEST_CASE( _version_ )
{
	unsigned short	major, minor, revision;
	unsigned	build;
	char		verStr[MAX_STRING];

	srand((unsigned)time(0));
	major = (unsigned short)( rand() % USHRT_MAX );
	minor = (unsigned short)( rand() % USHRT_MAX );
	revision = (unsigned short)( rand() % USHRT_MAX );
	build = (unsigned)( rand() % UINT_MAX );

	_SMERP::Version ver( major, minor, revision, build );
	sprintf( verStr, "%d.%d.%d.%u", major, minor, revision, build );

	BOOST_CHECK_EQUAL( ver.major(), major );
	BOOST_CHECK_EQUAL( ver.minor(), minor );
	BOOST_CHECK_EQUAL( ver.revision(), revision );
	BOOST_CHECK_EQUAL( ver.build(), build );
	BOOST_CHECK_EQUAL( ver.toString().c_str(), verStr );
	std::cout << ver.toString();
}

