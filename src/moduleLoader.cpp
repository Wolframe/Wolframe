/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
/// \file moduleLoader.cpp

#include "logger-v1.hpp"
#include "module/moduleLoader.hpp"
#include "utils/fileUtils.hpp"

#if !defined(_WIN32)	// POSIX module loader

#include <dlfcn.h>

typedef	void*	_Wolframe_MODULE_HANDLE;
#define	_Wolframe_DLL_CLOSE(x)	dlclose( x )
#else		// Win32 module loader

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <string.h>

typedef	HMODULE	_Wolframe_MODULE_HANDLE;
#define	_Wolframe_DLL_CLOSE(x)	(void)FreeLibrary( x )

#ifdef LOCAL_ERROR_BUFFER_SIZE
#error "LOCAL_ERROR_BUFFER_SIZE previously defined"
#else
#define	LOCAL_ERROR_BUFFER_SIZE	512
#endif
char *getLastError( char *buf, size_t buflen )
{
	LPTSTR errbuf;
	DWORD errbuf_len;
	DWORD res;
	DWORD last_error;

	last_error = GetLastError( );

	res = FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL,			/* message is from system */
				last_error,		/* there is a message with that id */
				0,			/* default language preference */
				(LPTSTR)&errbuf,	/* buffer allocated internally with LocalAlloc */
				0,			/* minimum allocation size */
				NULL );			/* no arguments */

	if( res == 0 ) {
		strncpy( buf, "No message available", buflen );
	} else {
		strncpy( buf, errbuf, buflen );
		LocalFree( errbuf );
	}

	return buf;
}

#endif		// defined(_WIN32)

namespace {
class LocalGarbageCollector
{
public:
	~LocalGarbageCollector()	{
		while ( !m_handle.empty())	{
			_Wolframe_DLL_CLOSE( m_handle.back());
			m_handle.pop_back();
		}
	}

	void addHandle( _Wolframe_MODULE_HANDLE handle )	{ m_handle.push_back( handle ); }
private:
	std::list< _Wolframe_MODULE_HANDLE >	m_handle;

};
}// anonymous namespace

static LocalGarbageCollector g_handleList;

using namespace _Wolframe;
using namespace _Wolframe::module;

ModuleEntryPoint* module::loadModuleEntryPoint( const std::string& absoluteModulePath)
{
	LOG_TRACE << "Loading module '" << absoluteModulePath << "'";
#if !defined(_WIN32)	// POSIX module loader
	std::string absoluteModulePath_ = absoluteModulePath;
	if (utils::getFileExtension( absoluteModulePath_).empty())
	{
		absoluteModulePath_.append( ".so");
	}
	void* hndl = dlopen( absoluteModulePath_.c_str(), RTLD_NOW | RTLD_LOCAL );
	if ( !hndl )	{
		LOG_ERROR << "Module loader: " << dlerror()
			  << ", (while loading module '" << absoluteModulePath_ << "')";
		return 0;
	}

	ModuleEntryPoint* entry = (ModuleEntryPoint*)dlsym( hndl, "entryPoint" );
	if ( !entry )	{
		LOG_ERROR << "Module entry point not found: " << dlerror()
			  << ", (module '" << absoluteModulePath_ << "')";
		dlclose( hndl );
		return 0;
	}
#else
	HMODULE hndl = LoadLibrary( absoluteModulePath.c_str( ) );
	if ( !hndl )	{
		char buf[LOCAL_ERROR_BUFFER_SIZE];
		LOG_ERROR << "Module loader: " << getLastError( buf, LOCAL_ERROR_BUFFER_SIZE )
			  << ", (module '" << absoluteModulePath << "')";
		return 0;
	}
	ModuleEntryPoint* entry = (ModuleEntryPoint*)GetProcAddress( hndl, "entryPoint" );
	if ( !entry )	{
		char buf[LOCAL_ERROR_BUFFER_SIZE];
		LOG_ERROR << "Module entry point not found: " << getLastError( buf, LOCAL_ERROR_BUFFER_SIZE )
			  << ", (module '" << absoluteModulePath << "')";
		(void)FreeLibrary( hndl );
		return 0;
	}
#endif
	if( !entry->name ) {
		LOG_ERROR << "Module entry point has no name, something is here '" << absoluteModulePath << "')";
		_Wolframe_DLL_CLOSE( hndl );
		return 0;
	}

	g_handleList.addHandle( hndl );
	LOG_DEBUG << "Module '" << entry->name << "' loaded";
	return entry;
}


