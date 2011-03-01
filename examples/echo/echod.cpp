//
// echod.cpp
//

// Super main function :)


#if defined(_WIN32)		// we are on Windows

	int _Wolframe_winMain( int argc, char* argv[] );

	int main( int argc, char* argv[] )
	{
		return ( _Wolframe_winMain( argc, argv ));
	}

#else			// we are on a POSIX system

	int _Wolframe_posixMain( int argc, char* argv[] );

	int main( int argc, char* argv[] )
	{
		return( _Wolframe_posixMain( argc, argv ));
	}
#endif // !defined(_WIN32)

