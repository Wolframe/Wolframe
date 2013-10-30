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
///\brief Internal interface for compiled database commands in the transaction function
///\file transactionfunction/DatabaseCommand.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_DATABASE_COMMAND_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_DATABASE_COMMAND_HPP_INCLUDED
#include "transactionfunction/Path.hpp"
#include "types/keymap.hpp"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace _Wolframe {
namespace db {
///\class DatabaseCommand
///\brief Database instruction call
class DatabaseCommand
{
public:
	///\brief Default constructor
	DatabaseCommand()
		:m_resultsetidx(-1)
		,m_nonemptyResult(false)
		,m_uniqueResult(false){}
	///\brief Copy constructor
	DatabaseCommand( const DatabaseCommand& o)
		:m_name(o.m_name)
		,m_selector(o.m_selector)
		,m_resultsetidx(o.m_resultsetidx)
		,m_arg(o.m_arg)
		,m_nonemptyResult(o.m_nonemptyResult)
		,m_uniqueResult(o.m_uniqueResult)
		,m_level(o.m_level)
		,m_hints(o.m_hints){}

	///\brief Constructor
	DatabaseCommand( const std::string& name_, const Path& selector_, int resultsetidx_, const std::vector<Path>& arg_, bool setNonemptyResult_, bool setUniqueResult_, std::size_t level_, const types::keymap<std::string>& hints_=types::keymap<std::string>())
		:m_name(name_)
		,m_selector(selector_)
		,m_resultsetidx(resultsetidx_)
		,m_arg(arg_)
		,m_nonemptyResult(setNonemptyResult_)
		,m_uniqueResult(setUniqueResult_)
		,m_level(level_)
		,m_hints(hints_)
	{}

	const Path& selector() const					{return m_selector;}
	const std::vector<Path>& arg() const				{return m_arg;}
	const std::string& name() const					{return m_name;}

	int resultsetidx() const					{return m_resultsetidx;}
	bool hasNonemptyResult() const					{return m_nonemptyResult;}
	bool hasUniqueResult() const					{return m_uniqueResult;}
	std::size_t level() const					{return m_level;}

	const char* getErrorHint( const std::string& errorclass) const	{types::keymap<std::string>::const_iterator hi = m_hints.find( errorclass); return (hi==m_hints.end())?0:hi->second.c_str();}

	std::string tostring() const
	{
		std::ostringstream rt;
		rt << "FOREACH "<< m_selector.tostring() << " CALL '" << m_name << "'( ";
		std::vector<Path>::const_iterator ai = m_arg.begin(), ae = m_arg.end();
		int ii = 0;
		for (; ai != ae; ++ai,++ii)
		{
			if (ii) rt << ", ";
			rt << ai->tostring();
		}
		rt << " )";
		return rt.str();
	}
private:
	std::string m_name;
	Path m_selector;
	int m_resultsetidx;
	std::vector<Path> m_arg;
	bool m_nonemptyResult;
	bool m_uniqueResult;
	std::size_t m_level;
	types::keymap<std::string> m_hints;
};

}}//namespace
#endif

