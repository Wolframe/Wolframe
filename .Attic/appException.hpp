//
// application exception class
//

#ifndef _APP_EXCEPTION_HPP_INCLUDED
#define _APP_EXCEPTION_HPP_INCLUDED


#include <string>
#include <exception>

#include "ErrorCode.hpp"

namespace _Wolframe	{

class appException : public std::exception
{
public:
	appException( ErrorCode::Error error, ErrorSeverity::Severity severity, const char* Format, ... );
	~appException() throw ()	{};

	ErrorCode::Error error()			{ return error_; };
	ErrorSeverity::Severity severity( void )	{ return severity_; }
	const char* what() const throw()		{ return what_.c_str(); }
	std::string docRepoAnswer( void );
private:
	ErrorCode::Error	error_;
	ErrorSeverity::Severity	severity_;
	std::string		what_;
};

template <typename C> inline std::basic_ostream<C>& operator<< ( std::basic_ostream<C>& o,
								appException const& e )
{
	return o << e.what();
}


class systemException : public std::exception
{
public:
	systemException( ErrorCode::Error error, ErrorSeverity::Severity severity,
		ErrorModule::Module module = ErrorModule::UNKNOWN, const char* msg = NULL );
	~systemException() throw ()	{};

	ErrorCode::Error error()			{ return error_; };
	ErrorSeverity::Severity severity()		{ return severity_; }
	ErrorModule::Module module()			{ return module_; }
	const char* msg()				{ return msg_; }
	const char* what() const throw ()		{ return msg_; }
private:
	ErrorCode::Error		error_;
	ErrorSeverity::Severity		severity_;
	ErrorModule::Module		module_;
	const char*			msg_;
};

template <typename C> inline std::basic_ostream<C>& operator<< ( std::basic_ostream<C>& o,
								systemException const& e )
{
	return o << e.what();
}

} // namespace _Wolframe

#endif // _APP_EXCEPTION_HPP_INCLUDED
