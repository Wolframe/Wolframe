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
///\file interfaceCommandHandler.cpp
#include "interfaceCommandHandler.hpp"
#include "logger-v1.hpp"
#include "processor/procProvider.hpp"
#include <stdexcept>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

struct IchSTM :public cmdbind::LineCommandHandlerSTMTemplate<InterfaceCommandHandler>
{
	enum State
	{
		User,
		Admin,
		AdminPush
	};

	IchSTM()
	{
		(*this)
			[User]
				.cmd< &InterfaceCommandHandler::doCapabilities >( "CAPABILITIES")
			[Admin]
				.cmd< &InterfaceCommandHandler::doCapabilities >( "CAPABILITIES")
		;
	}
};
static IchSTM ichstm;

InterfaceCommandHandler::InterfaceCommandHandler( bool adminInterface_)
	:cmdbind::LineCommandHandlerTemplate<InterfaceCommandHandler>( &ichstm, (std::size_t)(adminInterface_?IchSTM::User:IchSTM::Admin))
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





