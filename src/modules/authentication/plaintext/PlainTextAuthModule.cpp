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
// Plain Text Authentication module
//

#include "PlainTextAuth.hpp"
#include "moduleInterface.hpp"
#include "logger-v1.hpp"

_Wolframe::log::LogBackend*	logBackendPtr;

namespace _Wolframe {
namespace module {

static ContainerBuilder* createModule( void )
{
	static module::ContainerDescription< AAAA::PlainTextAuthContainer,
			AAAA::PlainTextAuthConfig > mod( "Plain text authentication", "Authentication",
							 "file", "PlainTextAuth" );
	return &mod;
}

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend* >( logger );
}


static const unsigned short nrContainers = 1;
static ContainerBuilder* (*containers[ nrContainers ])() = {
		createModule
};

ModuleEntryPoint entryPoint( 0, "Plain text authentication", setModuleLogger,
			     nrContainers, containers,
			     0, NULL );

}} // namespace _Wolframe::module