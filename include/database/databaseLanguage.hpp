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
///\file databaseLanguage.hpp
///\brief Language definitions for embedded commands in transactions
#ifndef _DATABASE_LANGUAGE_HPP_INCLUDED
#define _DATABASE_LANGUAGE_HPP_INCLUDED
#include "database/transaction.hpp"
#include "types/keymap.hpp"
#include "processor/userInterface.hpp"
#include <string>
#include <iostream>
#include <sstream>

namespace _Wolframe {
namespace db {

struct LanguageDescription
{
	LanguageDescription(){}
	virtual ~LanguageDescription(){}

	///\brief Operator marking the start of an end of line comment.
	// Default is the SQL standard EOLN comment marker
	virtual const char* eoln_commentopr() const
	{
		return "--";
	}

	///\brief String used for declaring a reference to an argument by index (starting with 1).
	// Default is the SQL standard argument reference in embedded database statements.
	virtual std::string stm_argument_reference( int index) const;

	virtual bool isEmbeddedStatement( std::string::const_iterator si, std::string::const_iterator se) const;

	virtual std::string parseEmbeddedStatement( std::string::const_iterator& si, std::string::const_iterator se) const;

	typedef std::pair<std::string,std::string> TemplateArgumentAssignment;
	virtual std::string substituteTemplateArguments( const std::string& cmd, const std::vector<TemplateArgumentAssignment>& arg) const;

	///\brief Define if a database is case-insensitive. This has influence on TDL parsing
	// Default is according SQL standard 'false'
	virtual bool isCaseSensitive() const
	{
		return false;
	}
};

}} // namespace _Wolframe::db

#endif

