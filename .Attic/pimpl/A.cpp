#include "A.hpp"
#include "AImpl.hpp"
#include "BImpl.hpp"

A::A( ) : impl( new BImpl<A::AImpl> )
{
}

A::~A( )
{
	delete impl;
}

int A::f( const int x )
{
	return impl->f( x );
}

void A::print( const int x ) const
{
	return impl->print( x );
}

void A::print( const char*& s ) const
{
	return impl->print( s );
}
