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
///\file mainCommandHandler.hpp

#ifndef _Wolframe_MAIN_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_MAIN_COMMAND_HANDLER_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include "cmdbind/lineCommandHandler.hpp"
#include "doctypeFilterCommandHandler.hpp"
#include "interfaceCommandHandler.hpp"
#include "processor/procProviderInterface.hpp"
#include "processor/execContext.hpp"
#include "AAAA/authentication.hpp"
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace proc {

class MainCommandHandler
	:public cmdbind::LineCommandHandlerTemplate<MainCommandHandler>
{
public:
	typedef cmdbind::LineCommandHandlerTemplate<MainCommandHandler> Parent;
	MainCommandHandler();
	virtual ~MainCommandHandler(){}

public:
	int doAuth( int argc, const char** argv, std::ostream& out);

	int doMech( int argc, const char** argv, std::ostream& out);
	int endMech( cmdbind::CommandHandler* ch, std::ostream& out);

	int doRequest( int argc, const char** argv, std::ostream& out);
	int endRequest( cmdbind::CommandHandler* ch, std::ostream& out);

	int doInterface( int argc, const char** argv, std::ostream& out);
	int endInterface( cmdbind::CommandHandler* ch, std::ostream& out);

	int doCapabilities( int argc, const char** argv, std::ostream& out);
	int doQuit( int argc, const char** argv, std::ostream& out);

	int endDoctypeDetection( cmdbind::CommandHandler* ch, std::ostream& out);
	int endErrDocumentType( cmdbind::CommandHandler* ch, std::ostream& out);

private:
	bool redirectConsumedInput( cmdbind::DoctypeFilterCommandHandler* fromh, cmdbind::CommandHandler* toh, std::ostream& out);

private:
	boost::shared_ptr<AAAA::Authenticator> m_authenticator;
	std::string m_command;
	std::string m_commandtag;
};

}}//namespace
#endif
