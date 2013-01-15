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
///\file ModuleLoader.cpp
///\brief Module loader for Windows
#include "ModuleLoader_Windows.hpp"
#include <Windows.h>

using namespace _Wolframe;
using namespace module;

typedef Module* (*CreateFunction)();

typedef struct SystemData
{
	HMODULE mhnd;
} SystemData;

Module* loadModule( const char* filename, long* errorCode)
{
	enum {MaxFileNameSize=511};
	wchar_t filenameW[ MaxFileNameSize+1];
	unsigned int ii;

	for (ii=0; ii<MaxFileNameSize && filename[ii]; ii++)
	{
		if ((signed char)(filename[ii]) <= 0)
		{
			*errorCode = -1;
			return 0;
		}
		filenameW[ ii] = filename[ ii];
	}
	if (filename[ii])
	{
		*errorCode = -2;
		return 0;
	}
	filenameW[ ii] = 0;
	HMODULE mhnd = LoadLibrary( filenameW);
	if (!mhnd)
	{
		*errorCode = GetLastError();
		return 0;
	}
	CreateFunction create = (CreateFunction)GetProcAddress( mhnd, "create");
	if (!create)
	{
		FreeLibrary( mhnd);
		*errorCode = GetLastError();
		return 0;
	}
	Module* rt = create();
	if (!rt)
	{
		*errorCode = GetLastError();
		FreeLibrary( mhnd);
		return 0;
	}
	((SystemData*) &rt->_private)->mhnd = mhnd;
	return rt;
}

void unloadModule( Module* module)
{
	if (module)
	{
		HMODULE mhnd = ((SystemData*) &module->_private)->mhnd;
		module->destroy( module);
		FreeLibrary( mhnd);
	}
}
