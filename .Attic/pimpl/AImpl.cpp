#include "AImpl.hpp"

#include <iostream>

A::AImpl::AImpl( )
{
}

A::AImpl::~AImpl( )
{
}

int A::AImpl::f( const int x )
{
	return x * 2;
}

void A::AImpl::print( const int x ) const
{
	std::cout << "an integer: " << x << std::endl;
}

void A::AImpl::print( const char*& s ) const
{
	std::cout << "a string: " << s << std::endl;
}
