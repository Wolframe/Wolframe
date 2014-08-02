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
//
// moduleDirectory_loadModules.cpp
//
#error DEPRECATED

#include "logger-v1.hpp"
#include "module/moduleDirectory.hpp"
#include "module/moduleInterface.hpp"
#include "module/moduleLoader.hpp"
#include "utils/fileUtils.hpp"

using namespace _Wolframe;
using namespace _Wolframe::module;

bool module::ModulesDirectory::loadModules( const std::list< std::string >& modFiles)
{
	bool retVal = true;

	for ( std::list< std::string >::const_iterator it = modFiles.begin();
							it != modFiles.end(); it++ )
	{
		ModuleEntryPoint* entry = loadModuleEntryPoint( *it);
		if ( !entry )
		{
			LOG_ERROR << "Failed to load module '" << *it << "'";
			retVal = false;
			break;
		}
		for ( unsigned short i = 0; entry->createBuilder[ i ]; i++ )
		{
			BuilderBase* builder = entry->createBuilder[ i ]();
			SimpleBuilder* simpleBuilder = dynamic_cast<SimpleBuilder*>(builder);
			ConfiguredBuilder* configuredBuilder = dynamic_cast<ConfiguredBuilder*>(builder);
			if (configuredBuilder)
			{
				addBuilder( configuredBuilder);
			}
			else if (simpleBuilder)
			{
				addBuilder( simpleBuilder);
			}
			else
			{
				LOG_ERROR << "Unknown type of builder in module '" << entry->name << "'";
			}
		}
		handleList.addHandle( hndl );
		LOG_DEBUG << "Module '" << entry->name << "' loaded";
	}
	return retVal;
}


