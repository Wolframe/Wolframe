//
// test if the compiler does the named return value optimization
//
// If the compiler does the optimization, the output should be just:
// foobar::foobar()
// foobar::~foobar()
//
// Most of the compilers should do that these days, but let's test...

#include <iostream>

class foobar	{
public:
	foobar()		{ std::cout << "foobar::foobar()\n"; }
	~foobar()		{ std::cout << "foobar::~foobar()\n"; }

	foobar( const foobar &rhs )
				{ std::cout << "foobar::foobar( const foobar & )\n"; }
	foobar& operator= ( const foobar &rhs )
				{ std::cout << "foobar::operator=( const foobar & )\n"; }

	void ival( int nval )	{ _ival = nval; }
private:
	int _ival;
};

foobar f( int val )
{
	foobar local;
	local.ival( val );
	return local;
}

int main()
{
	foobar ml = f( 1024 );
	return 0;
}

