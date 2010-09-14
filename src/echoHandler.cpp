//
// connectionHandler.cpp
//

#include "connectionHandler.hpp"

#include <string>


namespace _SMERP {

void connectionHandler::handleRequest( request& req, reply& rep )
{
	std::string msg = req.getValue();
	msg += '\n';
	rep.setValue( msg );
}


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



std::vector<boost::asio::const_buffer> reply::toBuffers()
{
  std::vector<boost::asio::const_buffer> buffers;

  buffers.push_back( boost::asio::buffer( content_ ));
  return buffers;
}



} // namespace _SMERP
