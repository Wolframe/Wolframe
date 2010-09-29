//
// miscStruct.hpp
//

#ifndef _MISCSTRUCT_HPP_INCLUDED
#define _MISCSTRUCT_HPP_INCLUDED

#include <string>

namespace _SMERP {

	/// structure for local endpoint configuration
	struct localEndpoint	{
		std::string	host;
		unsigned short	port;

		localEndpoint ( std::string h, unsigned short p )	{ host = h, port = p; }
	};

	/// structure for local SSL endpoint configuration
	struct localSSLendpoint	{
		std::string	host;
		unsigned short	port;
		std::string	certFile;
		std::string	keyFile;
		std::string	CAdirectory;
		std::string	CAchainFile;
		bool		verify;

		localSSLendpoint ( std::string h, unsigned short p )	{ host = h, port = p; }
	};

} // namespace _SMERP

#endif // _MISCSTRUCT_HPP_INCLUDED
