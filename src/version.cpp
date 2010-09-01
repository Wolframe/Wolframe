//
// version.cpp
//

#include <string>
#include <boost/lexical_cast.hpp>
#include "version.hpp"

namespace _SMERP {

Version::Version( unsigned short M )
{
	major_ = M;
	hasMinor_ = hasRevision_ = hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m )
{
	major_ = M, minor_ = m;
	hasMinor_ = true;
	hasRevision_ = hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m, unsigned short r )
{
	major_ = M, minor_ = m, revision_ = r;
	hasMinor_ = hasRevision_ = true;
	hasBuild_ = false;
}

Version::Version( unsigned short M, unsigned short m, unsigned short r, unsigned b )
{
	major_ = M, minor_ = m, revision_ = r, build_ = b;
	hasMinor_ = hasRevision_ = hasBuild_ = true;
}

//bool Version::operator== ( const Version &other ) const;

//bool Version::operator> ( const Version &other ) const;
//bool Version::operator>= ( const Version &other ) const;
//bool Version::operator< ( const Version &other ) const;
//bool Version::operator<= ( const Version &other ) const;

//std::string& toString();
} // namespace _SMERP

