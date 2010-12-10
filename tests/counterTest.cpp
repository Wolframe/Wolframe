//
// version class tests

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "atomic counter class"

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include "atomicCounter.hpp"

// this is because the echoHandler uses the logger, should it do that?
#include "logger.hpp"
_SMERP::LogBackend logBack;

#define	MAX_STRING	128

struct F {
	F()	{
		srand((unsigned)time(0));
		ulVal0 = (unsigned)( rand() % ULONG_MAX );
		iVal0 = (int)( rand() % INT_MAX );

		ulCounter0.set( ulVal0 );
		iCounter0.set( iVal0 );
		BOOST_TEST_MESSAGE( "setup initial test numbers" );
	}

	~F()	{
	}

	unsigned long	ulVal0;
	int		iVal0;
	_SMERP::AtomicCounter< unsigned long >	ulCounter0;
	_SMERP::AtomicCounter< int >		iCounter0;
};


BOOST_FIXTURE_TEST_SUITE( _atomicCounter_, F )

BOOST_AUTO_TEST_CASE( assignment )
{
	BOOST_CHECK_EQUAL( ulCounter0.val(), ulVal0 );
	BOOST_CHECK_EQUAL( iCounter0.val(), iVal0 );
}

BOOST_AUTO_TEST_CASE( operators )
{
}

BOOST_AUTO_TEST_SUITE_END()
