#include <string>

namespace _Wolframe {
namespace base64 {

std::string encode( unsigned char const*, size_t len);
std::string decode( std::string const& s );

}}
