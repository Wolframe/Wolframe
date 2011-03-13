#ifndef ERROR_UTILS_H
#define ERROR_UTILS_H

#include <iostream>

#include <strsafe.h>

#define GET_LAST_ERROR( last_error, errbuf, errbuf_size )				\
{											\
	LPVOID werrbuf;									\
	DWORD wbuf_size;								\
	DWORD wres;									\
											\
	wres = FormatMessage(								\
		FORMAT_MESSAGE_ALLOCATE_BUFFER |					\
		FORMAT_MESSAGE_FROM_SYSTEM |						\
		FORMAT_MESSAGE_IGNORE_INSERTS |						\
		FORMAT_MESSAGE_MAX_WIDTH_MASK,						\
		NULL,			/* message is from system */			\
		last_error,		/* there is a message with that id */		\
		0,			/* default language preference */		\
		(LPTSTR)&werrbuf,	/* buffer allocated internally with LocalAlloc */ 	\
		0,			/* minimum allocation size */			\
		NULL );			/* no arguments */				\
											\
	if( wres == 0 ) {								\
		StringCbCopy( errbuf, errbuf_size, _T( "No message available" ) );	 	\
	}										\
											\
	StringCbCopy( errbuf, errbuf_size, (LPCTSTR)werrbuf );					\
}

// define macros to switch to the correct output streams
#if defined(UNICODE) 
#define _tcout std::wcout
#define _tcerr std::wcerr
#else 
#define _tcout std::cout
#define _tcerr std::wcerr
#endif 

#define CERR_LAST_ERROR( msg )								\
	TCHAR errbuf[512];								\
	DWORD last_error = GetLastError( );						\
	GET_LAST_ERROR( last_error, errbuf, 512 );					\
	std::wcerr	<< "ERROR: " << msg << ", "						\
			<< errbuf << "(" << last_error << ")" << std::endl;

#endif
