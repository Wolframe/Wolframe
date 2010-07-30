//
// request.cpp
//

#include <string>
#include "request.hpp"


namespace _SMERP {


/// Parse some data. The return value indicates how much of the
/// input has been consumed.

char *request::parseInput( char *begin, std::size_t bytesTransferred )
{
	for ( std::size_t i = 0; i < bytesTransferred; i++ )	{
		if ( *begin != '\n' )
			content_ += *begin;
		else	{
			status_ = READY;
			return( begin );
		}
		begin++;
	}
	return( begin );
}

} // namespace _SMERP

