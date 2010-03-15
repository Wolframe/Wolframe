//
// version.cpp
//

#include <string>
#include <boost/lexical_cast.hpp>
#include "version.hpp"

namespace _SMERP {

Version::Version( unsigned short M, unsigned short m, unsigned short r, unsigned b )
{
	major_ = M, minor_ = m, revision_ = r, build_ = b;
	verStr_ += boost::lexical_cast<std::string>( major_ );
	verStr_ += ".";
	verStr_ += boost::lexical_cast<std::string>( minor_ );
	if ( r !=0 || b != 0 )	{
		verStr_ += ".";
		verStr_ += boost::lexical_cast<std::string>( revision_ );
		if ( b != 0 )	{
			verStr_ += ".";
			verStr_ += boost::lexical_cast<std::string>( build_ );
		}
	}
}

} // namespace _SMERP

