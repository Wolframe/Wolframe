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
///\brief Definition of transaction input/output objects
///\file database/transactionObjects.hpp
#ifndef _DATABASE_TRANSACTION_OBJECTS_HPP_INCLUDED
#define _DATABASE_TRANSACTION_OBJECTS_HPP_INCLUDED
#include <string>
#include <vector>
#include <cstdlib>

namespace _Wolframe {
namespace db {

struct TransactionInput
{
	struct Element
	{
		enum Type
		{
			String,
			ResultColumn
		};
		Type type;
		std::string value;
		std::size_t resultColumnIdx;

		Element(){}
		explicit Element( const std::string& value_)
			:type( String)
			,value(value_){}
		explicit Element( std::size_t resultColumnIdx_)
			:type( String)
			,resultColumnIdx(resultColumnIdx_){}
		Element( const Element& o)
			:type(o.type)
			,value(o.value)
			,resultColumnIdx(o.resultColumnIdx){}
	};

	struct Command
	{
		std::string name;
		std::vector<Element> arg;

		Command(){}
		explicit Command( const std::string& name_)
			:name(name_){}
		Command( const CommandResult& o)
			:name(o.name)
			,arg(o.arg){}
	};

	std::vector<Command> cmd;
};

struct TransactionOutput
{
	struct CommandResult
	{
		std::vector<string> columnName;
		std::vector< std::vector< std::string> > row;

		CommandResult(){}
		CommandResult( const CommandResult& o)
			:columnName(o.columnName)
			,row(o.row){}
	};

	std::vector<CommandResult> result;
};

}}
#endif


