//
// request.cpp
//

#include <string>
#include "request.hpp"


namespace _SMERP {


/// Parse some data. The return value indicates how much of the
/// input has been consumed.

char *request::parse( char *begin, char *end )
{
	while ( begin != end )	{
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

