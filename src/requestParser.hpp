//
// requestParser.hpp
//

#ifndef _REQUEST_PARSER_HPP_INCLUDED
#define _REQUEST_PARSER_HPP_INCLUDED

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace _SMERP {

class request;

/// Parser for incoming requests.
class request_parser
{
public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Parse some data. The tribool return value is true when a complete request
  /// has been parsed, false if the data is invalid, indeterminate when more
  /// data is required. The InputIterator return value indicates how much of the
  /// input has been consumed.
  template <typename InputIterator>
  boost::tuple<boost::tribool, InputIterator> parse(request& req,
      InputIterator begin, InputIterator end)
  {
    while (begin != end)
    {
      boost::tribool result = consume(req, *begin++);
      if (result || !result)
        return boost::make_tuple(result, begin);
    }
    boost::tribool result = boost::indeterminate;
    return boost::make_tuple(result, begin);
  }

private:
  /// Handle the next character of input.
  boost::tribool consume(request& req, char input);

};

} // namespace _SMERP

#endif // _REQUEST_PARSER_HPP_INCLUDED
