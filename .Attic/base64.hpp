#include <string>

namespace _Wolframe {
namespace base64 {

std::string base64_encode( unsigned char const*, size_t len);
std::string base64_decode( std::string const& s );

}}
