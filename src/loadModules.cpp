/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
//
// loadModules.cpp
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "processor/moduleInterface.hpp"
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

using namespace _Wolframe;

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

static LocalGarbageCollector	handleList;


bool _Wolframe::module::LoadModules( ModulesDirectory& modDir,
				     const std::list< std::string >& modFiles, const std::string & /*modFolder*/ )
{
	bool retVal = true;

	for ( std::list< std::string >::const_iterator it = modFiles.begin();
							it != modFiles.end(); it++ )	{
		LOG_TRACE << "Loading module '" << *it << "'";
#if !defined(_WIN32)	// POSIX module loader
		void* hndl;
		if ( utils::getFileExtension( *it).empty() )	{
			std::string path = *it + ".so";
			hndl = dlopen( path.c_str(), RTLD_LAZY );
		}
		else	{
			hndl = dlopen( it->c_str(), RTLD_LAZY );
		}
		if ( !hndl )	{
			LOG_ERROR << "Module loader: " << dlerror()
				  << ", (while loading module '" << *it << "')";
			retVal = false;
			break;
		}

		ModuleEntryPoint* entry = (ModuleEntryPoint*)dlsym( hndl, "entryPoint" );
		if ( !entry )	{
			LOG_ERROR << "Module entry point not found: " << dlerror()
				  << ", (module '" << *it << "')";
			retVal = false;
			dlclose( hndl );
			break;
		}
#else
		HMODULE hndl = LoadLibrary( it->c_str( ) );
		if ( !hndl )	{
			char buf[LOCAL_ERROR_BUFFER_SIZE];
			LOG_ERROR << "Module loader: " << getLastError( buf, LOCAL_ERROR_BUFFER_SIZE )
				  << ", (module '" << *it << "')";
			retVal = false;
			break;
		}
		ModuleEntryPoint* entry = (ModuleEntryPoint*)GetProcAddress( hndl, "entryPoint" );
		if ( !entry )	{
			char buf[LOCAL_ERROR_BUFFER_SIZE];
			LOG_ERROR << "Module entry point not found: " << getLastError( buf, LOCAL_ERROR_BUFFER_SIZE )
				  << ", (module '" << *it << "')";
			retVal = false;
			(void)FreeLibrary( hndl );
			break;
		}
#endif
		if( !entry->name ) {
			LOG_ERROR << "Module entry point has no name, something is here '" << *it << "')";
			retVal = false;
			_Wolframe_DLL_CLOSE( hndl );
			break;
		}

		entry->setLogger( &_Wolframe::log::LogBackend::instance() );
		for ( unsigned short i = 0; i < entry->cfgdContainers; i++ )	{
			modDir.addBuilder( entry->createCfgdBuilder[ i ]() );
		}
		for ( unsigned short i = 0; i < entry->containers; i++ )	{
			modDir.addBuilder( entry->createBuilder[ i ]() );
		}
		handleList.addHandle( hndl );
		LOG_DEBUG << "Module '" << entry->name << "' loaded";
	}
	return retVal;
}


