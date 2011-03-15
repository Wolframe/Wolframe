//
// appException.cpp
//

#include <string>
#include <exception>
#include <cstdarg>
#include <cstdio>

#include "ErrorCode.hpp"
#include "appException.hpp"


static const size_t WHAT_BUFFER_SIZE = 255;


namespace _Wolframe	{

appException::appException( ErrorCode::Error error, ErrorSeverity::Severity severity, const char* format, ... )
{
	char	buf[WHAT_BUFFER_SIZE];
	va_list	ap;

	error_ = error;
	severity_ = severity;

	va_start( ap, format );
	vsnprintf( buf, WHAT_BUFFER_SIZE, format, ap );
	va_end( ap );

	what_ = std::string( buf );
}


systemException::systemException( ErrorCode::Error error, ErrorSeverity::Severity severity,
				ErrorModule::Module module, const char* msg )
{
	error_ = error;
	severity_ = severity;
	module_ = module;
	msg_ = msg;
}

} // namespace _Wolframe

