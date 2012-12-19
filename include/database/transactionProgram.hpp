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
///\file database/transactionProgram.hpp
#ifndef _DATABASE_TRANSACTION_PROGRAM_HPP_INCLUDED
#define _DATABASE_TRANSACTION_PROGRAM_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/keymap.hpp"
#include "database/transactionFunction.hpp"
#include "database/databaseLanguage.hpp"
#include "utils/miscUtils.hpp"
#include <string>
#include <map>
#include <cstdlib>
#include <stdexcept>

namespace _Wolframe {
namespace db {

///\class TransactionProgram
///\brief Database program
class TransactionProgram
{
public:
	///\brief Constructor
	TransactionProgram()
		:m_langdescr(0){}

	///\brief Copy constructor
	TransactionProgram( const TransactionProgram& o)
		:m_functionmap(o.m_functionmap)
		,m_langdescr(o.m_langdescr){}

	///\brief Destructor
	~TransactionProgram(){}

	void defineEmbeddedLanguageDescription( const LanguageDescription* langdescr_)
	{
		m_langdescr = langdescr_;
	}

	///\brief Ask if the program is one of this class
	bool is_mine( const std::string& filename) const;

	///\brief Load transaction program source
	///\param[in] source source for database with included transaction definitions to parse
	///\param[out] dbsource source for database without transaction definitions
	///\remark Throws Program::Error exception
	std::vector<std::pair<std::string,TransactionFunctionR> >
		load( const std::string& source, std::string& dbsource, types::keymap<std::string>& embeddedStatementMap);

	///\brief Load transaction program source
	///\param[in] filename file with source for database with included transaction definitions to parse
	///\param[out] dbsource source for database without transaction definitions
	///\return return the new defined functions
	///\remark Throws std::runtime_error exception with positional error and filename
	std::vector<std::pair<std::string,TransactionFunctionR> >
		loadfile( const std::string& filename, std::string& dbsource, types::keymap<std::string>& embeddedStatementMap);

	///\brief Get a loaded function by name
	const TransactionFunction* function( const std::string& name) const;

private:
	///\brief Goto the next token ignoring comments
	char gotoNextToken( std::string::const_iterator& si, const std::string::const_iterator se) const;
	///\brief Parse the next token ignoring comments
	char parseNextToken( std::string& tok, std::string::const_iterator& si, std::string::const_iterator se) const;
	///\brief Parse embedded database statement in language defined with 'defineEmbeddedLanguageDescription(const LanguageDescription*)'
	std::pair<std::string,std::vector<std::string> >
		parseEmbeddedStatement( const std::string& funcname, int index, std::string::const_iterator& si, std::string::const_iterator se, types::keymap<std::string>& embeddedStatementMap);
	///\brief Parse statement of the form [identifier '(' arg { ',' arg } ')']
	std::pair<std::string,std::vector<std::string> >
		parseCallStatement( std::string::const_iterator& si, std::string::const_iterator se) const;

private:
	static const utils::CharTable m_optab;
	types::keymap<TransactionFunctionR> m_functionmap;
	const LanguageDescription* m_langdescr;
};

typedef types::CountedReference<TransactionProgram> TransactionProgramR;

}}//namespace
#endif

