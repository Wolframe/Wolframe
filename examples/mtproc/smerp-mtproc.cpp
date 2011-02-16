//
// smerpd.cpp
//

// Super main function :)


#if defined(_WIN32)		// we are on Windows

	int _SMERP_winMain( int argc, char* argv[] );

	int main( int argc, char* argv[] )
	{
		return ( _SMERP_winMain( argc, argv ));
	}

#else			// we are on a POSIX system

	int _SMERP_posixMain( int argc, char* argv[] );

	int main( int argc, char* argv[] )
	{
		return( _SMERP_posixMain( argc, argv ));
	}
#endif // !defined(_WIN32)

