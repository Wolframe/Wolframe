//
// logComponent.hpp
//

#ifndef _LOG_COMPONENT_HPP_INCLUDED
#define _LOG_COMPONENT_HPP_INCLUDED

#include <iostream>

namespace _Wolframe {

	class LogComponent {
	public:
		enum Component	{
			LOGCOMPONENT_NONE,		/// no loging component
			LOGCOMPONENT_LUA,		/// lua processor
			LOGCOMPONENT_NETWORK,		/// networking
			LOGCOMPONENT_AUTH,		/// authentication
			LOGCOMPONENT_DUMMY
		};

		static Component str2LogComponent( const std::string str );
	};


	// map enum values to strings
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
								  LogComponent::Component l )
	{
		static const CharT *const str[] = {
			"", "Lua", "Network", "Auth" };
		if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) ) {
			s << str[l];
		} else {
			s << "Unknown enum used '" << static_cast< int >( l ) << "' as log component";
		}
		return s;
	}

} // namespace _Wolframe


#endif // _LOG_COMPONENT_HPP_INCLUDED
