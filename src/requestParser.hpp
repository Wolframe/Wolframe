//
// requestParser.hpp
//

#ifndef REQUEST_PARSER_HPP_INCLUDED
#define REQUEST_PARSER_HPP_INCLUDED

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>

namespace _SMERP {

struct request;

/// Parser for incoming requests.
class request_parser
{
public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Reset to initial parser state.
  void reset();

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


  /// The current state of the parser.
  enum state
  {
    method_start,
    method,
    uri_start,
    uri,
    _SMERP_version_h,
    _SMERP_version_t_1,
    _SMERP_version_t_2,
    _SMERP_version_p,
    _SMERP_version_slash,
    _SMERP_version_major_start,
    _SMERP_version_major,
    _SMERP_version_minor_start,
    _SMERP_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
  } state_;
};

} // namespace _SMERP

#endif // REQUEST_PARSER_HPP_INCLUDED
