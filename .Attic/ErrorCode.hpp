//
// ErrorCode.hpp
//

#ifndef _ERRORCODES_HPP_INCLUDED
#define _ERRORCODES_HPP_INCLUDED

namespace _dR	{

	class ErrorCode	{
	public:
		enum Error	{
			OK = 0,
			FAILURE = 1,
			INVALID_CONFIGURATION,
			PARSING_ERROR = 100,
			MALFORMED_REQUEST,
			MALFORMED_ANSWER,
			MALFORMED_MESSAGE,
			OP_NOT_ALLOWED,
			INVALID_VALUE,
			INVALID_STATUS,
			DOES_NOT_EXIST,
			ALREADY_EXISTS,
			OBJECT_IN_USE,
			OUT_OF_MEMORY,
			FILESYSTEM_ERROR,
			FILE_OPEN_ERROR,
			MUTEX_ATTR_INIT,
			MUTEX_INIT,
			CONDITION_INIT,
			TIMEOUT,
// Errors above 1000 are warnings
			WARNING = 1000,
			NUMBER_NOT_GOOD,

			NON_FATAL_TIMEOUT,
			INTERNAL_ERROR,
			SYSTEM_ERROR,
			TRANSIENT_ERROR,
			RUNTIME_ERROR
		};
	};


	class ErrorSeverity	{
	public:
		enum Severity	{
			SEVERITY_NORMAL,	/// the system is working normally
			SEVERITY_WARNING,	/// ok, a warning is a warning
			SEVERITY_ERROR,		/// an error has occured, the system might be able to continue
			SEVERITY_SEVERE,	/// current processing is doomed. system not yet
			SEVERITY_CRITICAL,	/// the system is doomed now
			SEVERITY_ALERT,		/// run fast and tell the author
			SEVERITY_FATAL		/// system cannot continue
		};
	};


	class ErrorModule	{
	public:
		enum Module	{
			GLOBAL,			/// somewhere in the application
			REQUEST_PARSER,		/// InputMessage / Request
			DOCUMENT_PARSER,	/// document parser
			DEFINE_PARSER,		/// define request parser
			DOCUMENT_STORE,		/// storing the document
			REQUEST_HANDLER,
			DATABASE,
			NETWORK,
			LOGGER,			/// a logger manipulation error
			UNKNOWN			/// not known
		};
	};

} // namespace _dR


#endif // _ERRORCODES_HPP_INCLUDED
