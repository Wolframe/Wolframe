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

#ifndef _WIN32
#include <dlfcn.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

using namespace _Wolframe;

typedef module::ModuleContainer* (*CreateFunction)();
typedef void (*SetModuleLogger)( void* );

bool module::LoadModules( ModulesDirectory& modDir, std::list< std::string >& modFiles )
{
	bool retVal = true;

	for ( std::list< std::string >::const_iterator it = modFiles.begin();
							it != modFiles.end(); it++ )	{
#ifndef _WIN32
		void* hndl = dlopen( it->c_str(), RTLD_LAZY );
#else
		HMODULE hndl = LoadLibrary( it->c_str( ) );
#endif
		if ( !hndl )	{
#ifndef _WIN32
			LOG_ERROR << "Module loader: " << dlerror();
#else
			LOG_ERROR << "Module loader error";
#endif
			retVal = false;
			break;
		}

#ifndef _WIN32
		CreateFunction create = (CreateFunction)dlsym( hndl, "createModule" );
#else
		CreateFunction create = (CreateFunction)GetProcAddress( hndl, "createModule" );
#endif
		if ( !create )	{
#ifndef _WIN32
			LOG_ERROR << "Module loader creation entry point: " << dlerror();
#else
			LOG_ERROR << "Module loader creation entry point error";
#endif
			retVal = false;
#ifndef _WIN32
			dlclose( hndl );
#else
			(void)FreeLibrary( hndl );
#endif
			break;
		}

#ifndef _WIN32
		SetModuleLogger setLogger = (SetModuleLogger)dlsym( hndl, "setModuleLogger" );
#else
		SetModuleLogger setLogger = (SetModuleLogger)GetProcAddress( hndl, "setModuleLogger" );
#endif
		if ( !setLogger )	{
#ifndef _WIN32
			LOG_ERROR << "Module loader creation entry point: " << dlerror();
#else
			LOG_ERROR << "Module loader creation entry point error";
#endif
			retVal = false;
#ifndef _WIN32
			dlclose( hndl );
#else
			(void)FreeLibrary( hndl );
#endif
			break;
		}
		setLogger( &_Wolframe::log::LogBackend::instance() );
		modDir.addContainer( create() );
	}
	return retVal;
}

