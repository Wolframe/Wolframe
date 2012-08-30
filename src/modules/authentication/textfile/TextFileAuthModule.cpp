/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// Text File Authentication module
//

#include "TextFileAuth.hpp"
#include "moduleInterface.hpp"
#include "logger-v1.hpp"

_Wolframe::log::LogBackend*	logBackendPtr;

namespace _Wolframe {
namespace AAAA {

// Text file authentication - constructor
//***********************************************************************
class TextFileAuthConstructor : public ConfiguredObjectConstructor< AuthenticationUnit >
{
public:
	const char* identifier() const		{ return "TextFileAuth"; }
	TextFileAuthenticator* object( const config::NamedConfiguration& conf );
};


TextFileAuthenticator* TextFileAuthConstructor::object( const config::NamedConfiguration& conf )
{
	const TextFileAuthConfig& cfg = dynamic_cast< const TextFileAuthConfig& >( conf );

	TextFileAuthenticator* m_auth = new TextFileAuthenticator( cfg.m_identifier, cfg.m_file );
	MOD_LOG_DEBUG << "Text file authenticator container created for '"
		      << cfg.m_identifier << "'";
	return m_auth;
}

} // namespace AAAA

namespace module {

static ConfiguredBuilder* createModule( void )
{
	static module::ConfiguredBuilderDescription< AAAA::TextFileAuthConstructor,
			AAAA::TextFileAuthConfig > mod( "Authentication file", "Authentication",
							"TextFile", "TextFileAuth" );
	return &mod;
}

static void setModuleLogger( void* logger )
{
	logBackendPtr = reinterpret_cast< _Wolframe::log::LogBackend* >( logger );
}


static const unsigned short nrContainers = 1;
static ConfiguredBuilder* (*containers[ nrContainers ])() = {
	createModule
};

ModuleEntryPoint entryPoint( 0, "Text File authentication", setModuleLogger,
			     nrContainers, containers,
			     0, NULL );

} // namespace module
} // namespace _Wolframe
