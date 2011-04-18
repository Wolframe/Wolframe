namespace _Wolframe {
	namespace base64 {

#include <string>

std::string base64_encode( unsigned char const*, size_t len);
std::string base64_decode( std::string const& s );

}}
