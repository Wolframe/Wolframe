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
///\file cmdbind/discardInputCommandHandlerEscDLF.hpp
///\brief Command handler that discards the input (> /dev/null ) instead of processing it. Used for handling rest of content when an error occurrs before the whole input has been consumed.

#ifndef _Wolframe_cmdbind_DISCARD_INPUT_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#define _Wolframe_cmdbind_DISCARD_INPUT_COMMAND_HANDLER_ESC_DOT_LF_HPP_INCLUDED
#include "cmdbind/ioFilterCommandHandlerEscDLF.hpp"

namespace _Wolframe {
namespace cmdbind {

///\class DiscardInputCommandHandlerEscDLF
///\brief Command handler that does nothing but seek for the end of content marker. Used to delegate protocol input in case of an error in the protocol on a command that expects "LT.LF" terminated content.
class DiscardInputCommandHandlerEscDLF :public IOFilterCommandHandlerEscDLF
{
public:
	DiscardInputCommandHandlerEscDLF( const std::string& msg_)
		:m_msg(msg_){}
	virtual CallResult call( const char*& err)
	{
		err = m_msg.c_str(); return Error;
	}

private:
	std::string m_msg;
};

}}
#endif

