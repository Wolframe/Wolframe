#ifndef AIMPL_H
#define AIMPL_H

#include "A.hpp"

class A::AImpl {
	public:
		AImpl( );
		~AImpl( );

		int f( const int x );

		virtual void print( const int x ) const;
		virtual void print( const char*& s ) const;
};

#endif
