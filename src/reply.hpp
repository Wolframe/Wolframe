//
// reply.hpp
//

#ifndef _REPLY_HPP_INCLUDED
#define _REPLY_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace _SMERP {

	/// A reply to be sent to a client.
	class reply
	{
	friend class requestHandler;

	private:
		std::string content;		/// The content to be sent in the reply.

	public:
		/// Convert the reply into a vector of buffers. The buffers do not own the
		/// underlying memory blocks, therefore the reply object must remain valid and
		/// not be changed until the write operation has completed.
		std::vector<boost::asio::const_buffer> to_buffers();
	};


} // namespace _SMERP

#endif // _REPLY_HPP_INCLUDED
