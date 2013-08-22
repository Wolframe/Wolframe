/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\brief Internal interface for preprocessing function calls
///\file transactionfunction/PreProcessCommand.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_PRE_PROCESS_COMMAND_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_PRE_PROCESS_COMMAND_HPP_INCLUDED
#include "transactionfunction/Path.hpp"
#include <string>
#include <vector>

namespace _Wolframe {
namespace db {

///\class PreProcessCommand
///\brief Preprocessing function of transaction function
class PreProcessCommand
{
public:
	///\brief Default constructor
	PreProcessCommand(){}
	///\brief Copy constructor
	PreProcessCommand( const PreProcessCommand& o)
		:m_name(o.m_name),m_resultnames(o.m_resultnames),m_selector(o.m_selector),m_arg(o.m_arg){}
	///\brief Constructor
	PreProcessCommand( const std::string& name_, const std::vector<std::string>& resultnames_, const Path& selector_, const std::vector<Path>& arg_)
		:m_name(name_),m_resultnames(resultnames_),m_selector(selector_),m_arg(arg_){}

	const Path& selector() const					{return m_selector;}
	const std::vector<Path>& arg() const				{return m_arg;}
	const std::string& name() const					{return m_name;}
	const std::vector<std::string>& resultnames() const		{return m_resultnames;}

	void call( TransactionFunctionInput::Structure& structure);

private:
	std::string m_name;						//< name of the function (defined by the provider)
	std::vector<std::string> m_resultnames;				//< sequence of result names in case of redefining the returned structure element names
	Path m_selector;						//< FOREACH selector path
	std::vector<Path> m_arg;					//< function arguments
};

}}//namespace
#endif

