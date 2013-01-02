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
///\file interfaceCommandHandler.hpp
///\brief Interface to INTERFACE command handler (sub statemachine)
#ifndef _Wolframe_INTERFACE_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_INTERFACE_COMMAND_HANDLER_HPP_INCLUDED
#include "cmdbind/lineCommandHandler.hpp"
#include "UserInterface.hpp"
#include <list>
#include <string>

namespace _Wolframe {
namespace proc {

class InterfaceCommandHandler :public cmdbind::LineCommandHandlerTemplate<InterfaceCommandHandler>
{
public:
	typedef cmdbind::LineCommandHandlerTemplate<CommandHandler> Parent;
	explicit InterfaceCommandHandler( const std::list<std::string>& roles_);
	virtual ~InterfaceCommandHandler(){}

public:
	int doCapabilities( int argc, const char** argv, std::ostream& out);
	int doVersion( int argc, const char** argv, std::ostream& out);
	int doInfo( int argc, const char** argv, std::ostream& out);
	int doDescription( int argc, const char** argv, std::ostream& out);
	int doBody( int argc, const char** argv, std::ostream& out);
	int doDone( int argc, const char** argv, std::ostream& out);

private:
	bool initInterfaceObject( UI::InterfaceObject& obj, int argc, const char** argv, std::ostream& out);
	std::list<std::string> m_roles;
};

}}//namespace
#endif

