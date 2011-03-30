#ifndef A_H
#define A_H

class A {
	public:
		A( );
		~A( );

		int f( const int x );

	private:
		class AImpl;
		AImpl *impl;
};

#endif
