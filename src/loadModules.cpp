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

#if !defined(_WIN32)	// POSIX module loader

	#include <dlfcn.h>

	using namespace _Wolframe;

	typedef module::ModuleContainer* (*CreateFunction)();
	typedef void (*SetModuleLogger)( void* );

	bool module::LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles )
	{
		bool retVal = true;

		for ( std::list< std::string >::const_iterator it = modFiles.begin();
								it != modFiles.end(); it++ )	{
			void* hndl = dlopen( it->c_str(), RTLD_LAZY );
			if ( !hndl )	{
				LOG_ERROR << "Module loader: " << dlerror();
				retVal = false;
				break;
			}
			CreateFunction create = (CreateFunction)dlsym( hndl, "createModule" );
			if ( !create )	{
				LOG_ERROR << "Module loader creation entry point: " << dlerror();
				retVal = false;
				dlclose( hndl );
				break;
			}

			SetModuleLogger setLogger = (SetModuleLogger)dlsym( hndl, "setModuleLogger" );
			if ( !setLogger )	{
				LOG_ERROR << "Module loader creation entry point: " << dlerror();
				retVal = false;
				dlclose( hndl );
				break;
			}
			setLogger( &_Wolframe::log::LogBackend::instance() );
			modDir.addContainer( create() );
		}
		return retVal;
	}

#else		// Win32 module loader

	#define WIN32_MEAN_AND_LEAN
	#include <windows.h>

	using namespace _Wolframe;

	typedef module::ModuleContainer* (*CreateFunction)();
	typedef void (*SetModuleLogger)( void* );

	bool module::LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles )
	{
		bool retVal = true;

		for ( std::list< std::string >::const_iterator it = modFiles.begin();
								it != modFiles.end(); it++ )	{
			HMODULE hndl = LoadLibrary( it->c_str( ) );
			if ( !hndl )	{
				LOG_ERROR << "Module loader error";
				retVal = false;
				break;
			}
			CreateFunction create = (CreateFunction)GetProcAddress( hndl, "createModule" );
			if ( !create )	{
				LOG_ERROR << "Module loader creation entry point error";
				retVal = false;
				(void)FreeLibrary( hndl );
				break;
			}
			SetModuleLogger setLogger = (SetModuleLogger)GetProcAddress( hndl, "setModuleLogger" );
			if ( !setLogger )	{
				LOG_ERROR << "Module loader creation entry point error";
				retVal = false;
				(void)FreeLibrary( hndl );
				break;
			}
			setLogger( &_Wolframe::log::LogBackend::instance() );
			modDir.addContainer( create() );
		}
		return retVal;
	}

#endif		// defined(_WIN32)
