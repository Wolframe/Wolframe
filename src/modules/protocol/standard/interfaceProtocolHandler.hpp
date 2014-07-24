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
///\file interfaceProtocolHandler.hpp
///\brief Interface to INTERFACE command handler (sub statemachine)
#ifndef _Wolframe_INTERFACE_PROTOCOL_HANDLER_HPP_INCLUDED
#define _Wolframe_INTERFACE_PROTOCOL_HANDLER_HPP_INCLUDED
#include "lineProtocolHandler.hpp"
#include "processor/procProviderInterface.hpp"
#include <list>
#include <string>

namespace _Wolframe {
namespace cmdbind {

class InterfaceProtocolHandler :public cmdbind::LineProtocolHandlerTemplate<InterfaceProtocolHandler>
{
public:
	typedef cmdbind::LineProtocolHandlerTemplate<ProtocolHandler> Parent;
	explicit InterfaceProtocolHandler( bool adminInterface_=false);
	virtual ~InterfaceProtocolHandler(){}

public:
	int doCapabilities( int argc, const char** argv, std::ostream& out);

private:
	std::list<std::string> m_roles;
	std::vector<std::string> m_argbuf;
	std::vector<std::pair< std::string, std::string> > m_attributes;
};

}}//namespace
#endif

