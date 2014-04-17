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
///\brief Definition of a database instruction or subroutine call with FOREACH and INTO qualifier as command
///\file tdl/commandDefinition.hpp
#ifndef _DATABASE_TDL_COMMAND_DEFINITION_HPP_INCLUDED
#define _DATABASE_TDL_COMMAND_DEFINITION_HPP_INCLUDED
#include "database/databaseLanguage.hpp"
#include "tdl/subroutineCallStatement.hpp"
#include "tdl/embeddedStatement.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {
namespace tdl {

struct CommandDefinition
{
	struct Hint
	{
		std::string errorclass;
		std::string message;
		Hint(){}
		Hint( const Hint& o)
			:errorclass(o.errorclass),message(o.message){}
		Hint( const std::string& e, const std::string& m)
			:errorclass(e),message(m){}
	};

	std::string selector;
	EmbeddedStatement statement;
	SubroutineCallStatement call;
	std::vector<std::string> resultpath;
	bool nonempty;
	bool unique;
	bool embedded;
	std::vector<Hint> hints;

	CommandDefinition()
		:nonempty(false),unique(false),embedded(false){}
	CommandDefinition( const CommandDefinition& o)
		:selector(o.selector),statement(o.statement),call(o.call),resultpath(o.resultpath),nonempty(o.nonempty),unique(o.unique),embedded(o.embedded),hints(o.hints){}

	static CommandDefinition parse( const LanguageDescription* langdescr, std::string::const_iterator& si, const std::string::const_iterator& se);
};

}}}//namespace
#endif

