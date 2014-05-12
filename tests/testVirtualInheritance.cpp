//
// Test if the compilers support virtual inheritance on two inheritance pathes
//

#include <iostream>

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

using namespace std;

class Animal
{
	public:
		Animal( ) { cout << "Constructing animal.." << endl; }
		virtual ~Animal( ) { cout << "Destroying animal.." << endl; }

		virtual void fly( ) = 0;
		virtual void trot( ) = 0;
		
};

class Horse : public virtual Animal
{
	public:
		Horse( ) { cout << "Constructing horse.." << endl; }
		virtual ~Horse( ) { cout << "Destroying horse.." << endl; }
		virtual void trot( ) { cout << "I'm trotting" << endl; }
};

class Bird : public virtual Animal
{
	public:
		Bird( ) { cout << "Constructing bird.." << endl; }
		virtual ~Bird( ) { cout << "Destroying bird.." << endl; }
		virtual void fly( ) { cout << "I'm flying" << endl; }
};

class Pegasus : public virtual Horse, public virtual Bird
{
	public:
		Pegasus( ) { cout << "Constructing pegasus.." << endl; }
		virtual ~Pegasus( ) { cout << "Destroying pegasus.." << endl; }
};

TEST( VirtualInheritance, test )
{
	Animal *p = new Pegasus( );
	p->trot( );
	p->fly( );
	delete p;
}


int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

