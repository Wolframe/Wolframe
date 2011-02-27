//
// ErrorCodes.hpp
//

#ifndef _ERRORCODES_HPP_INCLUDED
#define _ERRORCODES_HPP_INCLUDED

namespace _Wolframe	{

	class ErrorCodes	{
	public:
		enum	{
			OK = 0,
			FAILURE = 1
		};
	};


	class ErrorLevel	{
	public:
		enum	{
			NORMAL,		/// the system is working normally
			WARNING,	/// ok, a warning is a warning
			/* Aba: gets expanded on Windows, ERROR is a macro! */
			_ERROR,		/// an error has occured, the system might be able to continue
			SEVERE,		/// current processing is doomed. system not yet
			CRITICAL,	/// the system is doomed now
			FATAL		/// system cannot continue
		};
	};

} // namespace _Wolframe


#endif // _ERRORCODES_HPP_INCLUDED
