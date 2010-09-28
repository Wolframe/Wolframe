#ifndef _NETWORK_MESSAGE_HPP_INCLUDED
#define _NETWORK_MESSAGE_HPP_INCLUDED

#include <cstddef>
#include <cstring>
#include <string>

namespace _SMERP {

	/// A message to be sent to a client.
	struct NetworkMessage
	{
	private:
		const void	*data_;
		std::size_t	size_;
	public:
		NetworkMessage()	{ data_ = NULL; size_ = 0; }
		NetworkMessage( const void *d, std::size_t s )
					{ data_ = d; size_ = s; }
		NetworkMessage( const char *str )
					{ data_ = str; size_ = std::strlen( str); }
		NetworkMessage( std::string& str )
					{ data_ = str.c_str(); size_ = str.length(); }

		const void* data()	{ return data_; }
		std::size_t size()	{ return size_; }
	};

} // namespace _SMERP

#endif // _NETWORK_MESSAGE_HPP_INCLUDED
