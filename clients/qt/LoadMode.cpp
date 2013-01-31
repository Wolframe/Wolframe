//
// LoadMode.cpp
//

#include "LoadMode.hpp"

QString LoadModeToStr( LoadMode m )
{
	switch( m ) {
		case Network: return QString( "Network" );
		case LocalFile: return QString( "LocalFile" );
		case Unknown: return QString( );
	}
	return QString( );
}

LoadMode LoadModeFromStr( const QString s )
{
	if( s == "Network" ) return Network;
	else if( s == "LocalFile" ) return LocalFile;
	return Unknown;
}
