#include "A.hpp"
#include "AImpl.hpp"

A::A( ) : impl( new AImpl )
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
