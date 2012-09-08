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
///\file cmdbind/commandHandlerUnit.hpp
///\brief Interface to command handler units

#ifndef _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#define _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED

#include "cmdbind/commandHandler.hpp"
#include <string>
#include <list>

namespace _Wolframe {
namespace cmdbind {

///\class CommandHandlerCreator
///\brief Set of command handlers adressable by identifiers
class CommandHandlerCreator
{
public:
	///\brief virtual destructor
	virtual ~CommandHandlerCreator()		{}

	///\brief Get an instance of the command handler for the command
	///\param [in] command the name of the command to be executed by the handler
	///\returns pointer to a CommandHandler object or NULL if none is available for command
	virtual CommandHandler* handler( const std::string& command );

	///\class const_iterator
	///\brief Get the list of commands for the unit
	///\returns pointer to a list of command names (strings). A NULL pointer is an error.
	virtual const std::list< std::string >* commands() const;
};

}} //namespace _Wolframe::cmdbind

#endif // _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
