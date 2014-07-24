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
///\file cmdbind/commandHandlerUnit.hpp
///\brief Interface to command handler units
#ifndef _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#define _Wolframe_CMDBIND_COMMAND_HANDLER_UNIT_HPP_INCLUDED
#include "cmdbind/commandHandler.hpp"
#include <boost/shared_ptr.hpp>
#include <string>
#include <list>

namespace _Wolframe {
namespace cmdbind {

/// \class CommandHandlerUnit
/// \brief Class that defines a command handler class and is able to create instances of it
class CommandHandlerUnit
{
public:
	/// \brief Load all configured programs
	virtual bool loadPrograms( const proc::ProcessorProviderInterface* provider)=0;

	/// \brief Get the list of all commands inplemented by this command handler
	virtual std::vector<std::string> commands() const=0;

	/// \brief Create an instance of this command handler
	virtual CommandHandler* createCommandHandler( const std::string& cmdname, const std::string& docformat)=0;
};

/// \brief Command handler unit reference
typedef boost::shared_ptr<CommandHandlerUnit> CommandHandlerUnitR;

}}//namespace
#endif


