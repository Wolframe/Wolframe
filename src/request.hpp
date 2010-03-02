//
// request.hpp
//

#ifndef _REQUEST_HPP_INCLUDED
#define _REQUEST_HPP_INCLUDED

#include <string>


namespace _SMERP {


// A request received from a client.
class request
{
private:
	std::string content_;

public:
	const std::string& getValue()	{ return content_; };
};


} // namespace _SMERP

#endif // _REQUEST_HPP_INCLUDED
