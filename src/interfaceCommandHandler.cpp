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
///\file interfaceCommandHandler.cpp
#include "interfaceCommandHandler.hpp"
#include "logger-v1.hpp"
#include "processor/procProvider.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

enum State
{
	Process
};

struct STM :public cmdbind::LineCommandHandlerSTMTemplate<InterfaceCommandHandler>
{
	STM()
	{
		(*this)
			[Process]
				.cmd< &InterfaceCommandHandler::doInfo >( "INFO")
				.cmd< &InterfaceCommandHandler::doDescription >( "DESCRIPTION")
				.cmd< &InterfaceCommandHandler::doBody >( "BODY")
				.cmd< &InterfaceCommandHandler::doDone >( "DONE")
				.cmd< &InterfaceCommandHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static STM stm;

static unsigned long getNumber( const char* aa)
{
	unsigned long result = 0;
	for (int ii=0; aa[ii] <= '9' && aa[0] >= '0'; ++ii)
	{
		unsigned long xx = result * 10 + aa[0] - '0';
		if (xx < result) throw std::runtime_error( "number out of range");
		result = xx;
	}
	return result;

}

InterfaceCommandHandler::InterfaceCommandHandler( const std::list<std::string>& roles_)
	:cmdbind::LineCommandHandlerTemplate<InterfaceCommandHandler>( &stm )
	,m_roles(roles_)
{}

int InterfaceCommandHandler::doCapabilities( int argc, const char**, std::ostream& out)
{
	if (argc != 0)
	{
		out << "ERR unexpected arguments" << endl();
		return stateidx();
	}
	else
	{
		out << "OK" << boost::algorithm::join( cmds(), " ") << endl();
		return stateidx();
	}
}

int InterfaceCommandHandler::doInfo( int argc, const char** argv, std::ostream& out)
{
	//INFO platform culture tag
	if (argc > 3)
	{
		out << "ERR too many arguments" << endl();
		return stateidx();
	}
	if (argc < 2)
	{
		out << "ERR too few arguments" << endl();
		return stateidx();
	}
	const char* platform = argv[0];
	const char* culture = argv[1];
	const char* tag = (argc == 2)?"":argv[2];
	const UI::UserInterfaceLibrary* uilib = m_provider->UIlibrary();
	std::list<UI::InterfaceObject::Info> selection = uilib->interface( platform, m_roles, culture, tag);

	std::list<UI::InterfaceObject::Info>::const_iterator ni = selection.begin(), ne = selection.end();
	for (; ni != ne; ++ni)
	{
		//INFO platform culture type name version
		out << "INFO " << ni->platform() << " " << ni->culture() << " " << ni->type() << " " << ni->name() << " " << ni->version().toString() << endl();
	}
	out << "OK" << endl();
	return stateidx();
}

bool InterfaceCommandHandler::initInterfaceObject( UI::InterfaceObject& obj, int argc, const char** argv, std::ostream& out)
{
	if (argc > 5)
	{
		out << "ERR too many arguments" << endl();
		return false;
	}
	if (argc < 4)
	{
		out << "ERR too few arguments" << endl();
		return false;
	}
	const char* platform = argv[0];
	const char* culture = argv[1];
	const char* type = argv[2];
	const char* name = argv[3];
	const char* versionstr = (argc == 4)?0:argv[4];
	Version version( versionstr );
	const UI::UserInterfaceLibrary* uilib = m_provider->UIlibrary();
	UI::InterfaceObject::Info info( type, platform, name, culture, version.toNumber(), "" );
	obj = uilib->object( info );
	return true;
}

int InterfaceCommandHandler::doDescription( int argc, const char** argv, std::ostream& out)
{
	//DESCRIPTION platform culture type name version
	UI::InterfaceObject obj;
	if (!initInterfaceObject( obj, argc, argv, out)) return stateidx();
	out << "DESCRIPTION" << endl();
	out << protocol::escapeStringDLF( obj.info().description());
	out << "OK" << endl();
	return stateidx();
}

int InterfaceCommandHandler::doBody( int argc, const char** argv, std::ostream& out)
{
	//BODY platform culture type name version
	UI::InterfaceObject obj;
	if (!initInterfaceObject( obj, argc, argv, out)) return stateidx();
	out << "DESCRIPTION" << endl();
	out << protocol::escapeStringDLF( obj.body());
	out << "OK" << endl();
	return stateidx();
}

int InterfaceCommandHandler::doDone( int argc, const char**, std::ostream& out)
{
	if (argc > 0)
	{
		out << "ERR too many arguments" << endl();
		return stateidx();
	}
	out << "DONE INTERFACE" << endl();
	return -1;
}





