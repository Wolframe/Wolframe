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
///\file database/program.hpp
//
#ifndef _DATABASE_PROGRAM_HPP_INCLUDED
#define _DATABASE_PROGRAM_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "database/transactionFunction.hpp"
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
	explicit Program( const char* commentopr_ = "--")
		:m_commentopr(commentopr_){}

	///\brief Copy constructor
	Program( const Program& o)
		:m_functionmap(o.m_functionmap)
		,m_commentopr(o.m_commentopr){}

	///\brief Destructor
	~Program(){}

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
	///\param[in] source source for database with included transaction definitions to parse
	///\param[out] dbsource source for database without transaction definitions
	///\remark Throws Program::Error exception
	void load( const std::string& source, std::string& dbsource);

	///\brief Load transaction program source
	///\param[in] filename file with source for database with included transaction definitions to parse
	///\param[out] dbsource source for database without transaction definitions
	///\remark Throws std::runtime_error exception with positional error and filename
	void loadfile( const std::string& filename, std::string& dbsource);

	///\brief Get a loaded function by name
	const TransactionFunction* function( const std::string& name) const;

private:
	std::map<std::string, TransactionFunctionR> m_functionmap;
	std::string m_commentopr;
};

typedef types::CountedReference<Program> ProgramR;

}}//namespace
#endif
