//
// connectionTimeout.hpp
//

#ifndef _CONNECTION_TIMEOUT_HPP_INCLUDED
#define _CONNECTION_TIMEOUT_HPP_INCLUDED

#include <iostream>

namespace _SMERP {

	struct connectionTimeout
	{
	public:
		enum TimeOutType	{
			TIMEOUT_NONE,
			TIMEOUT_IDLE,
			TIMEOUT_REQUEST,
			TIMEOUT_PROCESSING,
			TIMEOUT_ANSWER
		};

		unsigned long	idle;
		unsigned long	request;
		unsigned long	processing;
		unsigned long	answer;

		connectionTimeout()	{
			idle = request = processing = answer = 0;
		}

		connectionTimeout( unsigned long idleTO, unsigned long requestTO,
				   unsigned long processingTO, unsigned long answerTO )	{
			idle = idleTO;
			request = requestTO;
			processing = processingTO;
			answer = answerTO;
		}
	};


	// map enum values to strings
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
								  connectionTimeout::TimeOutType t )
	{
		static const char *const str[] = {
			"NONE", "IDLE", "REQUEST", "PROCESSING", "ANSWER" };
		if( static_cast< std::size_t >( t ) < ( sizeof( str ) / sizeof( *str ) ) ) {
			s << str[t];
		} else {
			s << "Unknown enum used '" << static_cast< int >( t ) << "'";
		}
		return s;
	}

} // namespace _SMERP

#endif // _CONNECTION_TIMEOUT_HPP_INCLUDED
