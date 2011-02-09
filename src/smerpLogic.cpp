//
// smerpLogic.cpp
//

#include <boost/algorithm/string.hpp>

#include "miscUtils.hpp"

namespace _SMERP {

	/// String to boolean functions
	bool isBoolean( const std::string& val )
	{
		std::string s = val;
		boost::to_upper( s );
		boost::trim( s );
		if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )
			return true;
		if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )
			return true;
		return false;
	}

	bool isTrue( const std::string& val )
	{
		std::string s = val;
		boost::to_upper( s );
		boost::trim( s );
		if ( s == "YES" || s == "TRUE" || s == "1" || s == "ON" )
			return true;
		return false;
	}

	bool isFalse( const std::string& val )
	{
		std::string s = val;
		boost::to_upper( s );
		boost::trim( s );
		if ( s == "NO" || s == "FALSE" || s == "0" || s == "OFF" )
			return true;
		return false;
	}

} // namespace _SMERP
