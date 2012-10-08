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
///\brief Interface to read a program for the database
///\file database/preparedStatement.hpp
#ifndef _DATABASE_PROGRAM_HPP_INCLUDED
#define _DATABASE_PROGRAM_HPP_INCLUDED
#include "langbind/transactionFunction.hpp"
#include "types/countedReference.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include <stdexcept>

namespace _Wolframe {
namespace db {

///\class Program
///\brief Database program
class Program
{
public:
	///\brief Constructor
	Program()
		:m_commentopr("--"){}
	///\brief Destructor
	virtual ~Program(){}

	void defineCommentOpr( const std::string& commentopr_)
	{
		m_commentopr = commentopr_;
	}

	struct LineInfo
	{
		LineInfo( unsigned int line_, unsigned int col_)
			:line(line_),col(col_){}
		LineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos);

		unsigned int line;
		unsigned int col;
	};

	class Error :public std::runtime_error
	{
	public:
		Error( const LineInfo& pos_, const std::string& msg_);
		Error( const LineInfo& pos_, const std::string& msg_, const std::string& arg_);
		Error( const LineInfo& pos_, const std::string& msg_, char arg_);
		virtual ~Error() throw(){}

		const char* what() const throw()
		{
			return m_msg.c_str();
		}
	private:
		std::string m_msg;
	};

	///\brief Load transaction program source
	///\remark Throws Program::Error exception
	void load( const std::string& source);

	///\brief Load of the database specific part of the program
	///\note default method checks, if source is empty after loading the transaction part and should be overloaded with some meaningful method
	virtual void loadDatabasePart( const std::string&);

	///\brief Get a loaded function by name
	const langbind::TransactionFunction* function( const std::string& name) const;

private:
	std::map<std::string, langbind::TransactionFunctionR> m_functionmap;
	std::string m_commentopr;
};

typedef types::CountedReference<Program> ProgramR;

}}//namespace
#endif

