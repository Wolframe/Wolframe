//
// version class tests

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "version class"

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include "version.hpp"

// this is because the echoHandler uses the logger, should it do that?
#include <logger.hpp>
_SMERP::LogBackend logBack;

#define	MAX_STRING	128

struct F {
	F()	{
		srand((unsigned)time(0));
		major = (unsigned short)( rand() % USHRT_MAX );
		minor = (unsigned short)( rand() % USHRT_MAX );
		revision = (unsigned short)( rand() % USHRT_MAX );
		build = (unsigned)( rand() % UINT_MAX );
		if ( !build )
			build++;

		ver = new _SMERP::Version( major, minor, revision, build );
		ver0 = new _SMERP::Version( major, minor, revision );
		sprintf( verStr, "%d.%d.%d.%u", major, minor, revision, build );

		BOOST_TEST_MESSAGE( "setup test versions" );
	}

	~F()	{
		delete ver0;
		delete ver;
		BOOST_TEST_MESSAGE( "teardown fixture" );
	}

	unsigned short	major, minor, revision;
	unsigned	build;
	_SMERP::Version	*ver, *ver0;
	char		verStr[MAX_STRING];
};


BOOST_FIXTURE_TEST_SUITE( _version_, F )

BOOST_AUTO_TEST_CASE( members )
{
	BOOST_CHECK_EQUAL( ver->major(), major );
	BOOST_CHECK_EQUAL( ver->minor(), minor );
	BOOST_CHECK_EQUAL( ver->revision(), revision );
	BOOST_CHECK_EQUAL( ver->build(), build );
	BOOST_CHECK_EQUAL( ver->toString().c_str(), verStr );
}

BOOST_AUTO_TEST_CASE( operators )
{
	BOOST_CHECK( *ver != *ver0 );
	BOOST_CHECK( *ver > *ver0 );
	BOOST_CHECK( *ver >= *ver0 );
	BOOST_CHECK( !( *ver == *ver0 ));
	BOOST_CHECK( !( *ver < *ver0 ));
	BOOST_CHECK( !( *ver <= *ver0 ));

	std::cout << ver->toString();
}

BOOST_AUTO_TEST_SUITE_END()
