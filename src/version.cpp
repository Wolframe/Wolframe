//
// version.cpp
//

#include <string>
#include <boost/lexical_cast.hpp>
#include "version.hpp"

namespace _SMERP {

version::version( unsigned short M, unsigned short m )
{
	major_ = M, minor_ = m;
	verStr += boost::lexical_cast<std::string>( major_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( minor_ );
}

version::version( unsigned short M, unsigned short m, unsigned short r )
{
	major_ = M, minor_ = m, revision_ = r;
	verStr += boost::lexical_cast<std::string>( major_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( minor_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( revision_ );
}

version::version( unsigned short M, unsigned short m, unsigned short r, unsigned b )
{
	major_ = M, minor_ = m, revision_ = r, build_ = b;
	verStr += boost::lexical_cast<std::string>( major_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( minor_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( revision_ );
	verStr += ".";
	verStr += boost::lexical_cast<std::string>( build_ );
}

} // namespace _SMERP

