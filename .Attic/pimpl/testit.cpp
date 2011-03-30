#include "A.hpp"
//#include "AImpl.hpp"

#include <iostream>

int main( )
{
	A a;
//	A::AImpl ai;

	std::cout << a.f( 47 ) << std::endl;
	a << 47;
	a << "bla";
//	std::cout << ai.f( 42 ) << std::endl;

	return 0;
}
