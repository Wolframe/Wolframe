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
///\file cmdbind/protocolCommandHandler.hpp
///\brief Interface to a command handler in the protocol context
#ifndef _Wolframe_CMDBIND_PROTOCOL_COMMAND_HANDLER_HPP_INCLUDED
#define _Wolframe_CMDBIND_PROTOCOL_COMMAND_HANDLER_HPP_INCLUDED
#include "countedReference.hpp"
#include "commandHandler.hpp"
#include <vector>
#include <string>

namespace _Wolframe {
namespace cmdbind {

class ProtocolCommandHandler :public CommandHandler
{
public:
	///\brief Defaul constructor
	ProtocolCommandHandler(){}

	///\brief Destructor
	virtual ~ProtocolCommandHandler(){}

	///\brief Pass the parameters for processing
	///\param [in] argc number of arguments of the protocol command
	///\param [in] argv arguments of the protocol command
	void passParameters( const std::string& nam, int argc, const char** argv);

protected:
	std::string m_name;				//< name of the command to execute
	std::vector< std::string > m_argBuffer;		//< the command arguments
};

typedef CountedReference<ProtocolCommandHandler> ProtocolCommandHandlerR;

}}
#endif

