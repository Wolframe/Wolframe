#ifndef A_H
#define A_H

class A {
	public:
		A( );
		~A( );

		int f( const int x );

		void print( const int x ) const;

	private:
		class AImpl;
		AImpl *impl;
};

template<typename T>
A& operator<<( A& a, T t )
{
	a.print( t );
	return a;
}

#endif
