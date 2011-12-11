/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
#include "moduleInterface.hpp"
#include "loadModules.hpp"

#if !defined(_WIN32)	// POSIX module loader

	#include <dlfcn.h>

	using namespace _Wolframe;

	typedef module::ModuleContainer* (*CreateFunction)();
	typedef void (*SetModuleLogger)( void* );

	class OpenDLlist
	{
	public:
		~OpenDLlist()	{
			while ( !m_handle.empty())	{
				dlclose( m_handle.back());
				m_handle.pop_back();
			}
		}

		void addHandle( void* handle )	{ m_handle.push_back( handle); }
	private:
		std::list< void* >	m_handle;

	};

	static OpenDLlist	handleList;

	bool module::LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles )
	{
		bool retVal = true;

		for ( std::list< std::string >::const_iterator it = modFiles.begin();
								it != modFiles.end(); it++ )	{
			LOG_TRACE << "Loading module '" << *it << "'";
			void* hndl = dlopen( it->c_str(), RTLD_LAZY );
			if ( !hndl )	{
				LOG_ERROR << "Module loader: " << dlerror()
					  << ", (module '" << *it << "')";
				retVal = false;
				break;
			}

			CreateFunction create = void_ptr_to_func_ptr_cast<CreateFunction>( dlsym( hndl, "createModule" ) );
			if ( !create )	{
				LOG_ERROR << "Module creation entry point: " << dlerror()
					  << ", (module '" << *it << "')";
				retVal = false;
				dlclose( hndl );
				break;
			}

			SetModuleLogger setLogger = void_ptr_to_func_ptr_cast<SetModuleLogger>( dlsym( hndl, "setModuleLogger" ) );
			if ( !setLogger )	{
				LOG_ERROR << "Module logging entry point: " << dlerror()
					  << ", (module '" << *it << "')";
				retVal = false;
				dlclose( hndl );
				break;
			}
			setLogger( &_Wolframe::log::LogBackend::instance() );
			modDir.addContainer( create() );
			handleList.addHandle( hndl );
		}
		return retVal;
	}

#else		// Win32 module loader

	#define WIN32_MEAN_AND_LEAN
	#include <windows.h>
	#include <string.h>

	using namespace _Wolframe;

	typedef module::ModuleContainer* (*CreateFunction)();
	typedef void (*SetModuleLogger)( void* );

	class OpenDLlist
	{
	public:
		~OpenDLlist()	{
			while ( !m_handle.empty())	{
				FreeLibrary( m_handle.back());
				m_handle.pop_back();
			}
		}

		void addHandle( void* handle )	{ m_handle.push_back( handle); }
	private:
		std::list< HMODULE >	m_handle;

	};

	static OpenDLlist	handleList;

	char *getLastError( char *buf, size_t buflen ) {
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

	bool module::LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles )
	{
		bool retVal = true;
		char buf[512];

		for ( std::list< std::string >::const_iterator it = modFiles.begin();
								it != modFiles.end(); it++ )	{
			LOG_TRACE << "Loading module '" << *it << "'";
			HMODULE hndl = LoadLibrary( it->c_str( ) );
			if ( !hndl )	{
				LOG_ERROR << "Module loader: " << getLastError( buf, 512 )
					  << ", (module '" << *it << "')";
				retVal = false;
				break;
			}
			CreateFunction create = (CreateFunction)GetProcAddress( hndl, "createModule" );
			if ( !create )	{
				LOG_ERROR << "Module creation entry point: " << getLastError( buf, 512 )
					  << ", (module '" << *it << "')";
				retVal = false;
				(void)FreeLibrary( hndl );
				break;
			}
			SetModuleLogger setLogger = (SetModuleLogger)GetProcAddress( hndl, "setModuleLogger" );
			if ( !setLogger )	{
				LOG_ERROR << "Module logging entry point: " << getLastError( buf, 512 )
					  << ", (module '" << *it << "')";
				retVal = false;
				(void)FreeLibrary( hndl );
				break;
			}
			setLogger( &_Wolframe::log::LogBackend::instance() );
			modDir.addContainer( create() );
			handleList.addHandle( hndl );
		}
		return retVal;
	}

#endif		// defined(_WIN32)
