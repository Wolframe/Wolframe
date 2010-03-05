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
public:
/// The status of the parser.
	enum parseStatus_t	{
		EMPTY,
		PARSING,
		READY
	};

private:
	std::string	content_;
	parseStatus_t	status_;
public:

	request()
	{
		status_ = EMPTY;
	}

/// Parse some data. The InputIterator return value indicates how much of the
/// input has been consumed.

template <typename InputIterator> InputIterator parse( InputIterator begin, InputIterator end )
{
	while ( begin != end )	{
		if ( *begin != '\n' )
			content_ += *begin;
		begin++;
	}
	return ( begin );
}

	std::string& getValue()	{ return content_; }
	parseStatus_t status() 	{ return status_; }

};

} // namespace _SMERP

#endif // _REQUEST_HPP_INCLUDED
