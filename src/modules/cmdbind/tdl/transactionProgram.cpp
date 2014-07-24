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
///\brief Implements the loading of programs in the transaction definition language
///\file prgbind_transactionProgram.cpp
#include "transactionProgram.hpp"
#include "loadTransactionProgram.hpp"
#include "prgbind/programLibrary.hpp"
#include "filter/redirectFilterClosure.hpp"
#include "database/databaseError.hpp"
#include "database/database.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::prgbind;

bool TransactionDefinitionProgram::is_mine( const std::string& filename) const
{
	return boost::algorithm::to_lower_copy( utils::getFileExtension( filename)) == ".tdl";
}

void TransactionDefinitionProgram::loadProgram( ProgramLibrary& library, db::Database* transactionDB, const std::string& filename)
{
	static const db::LanguageDescription defaultLanguageDescr;
	const db::LanguageDescription* languageDescr = (transactionDB)?transactionDB->getLanguageDescription():&defaultLanguageDescr;

	LOG_DEBUG << "Loading transaction program '" << filename << "':";

	try
	{
		std::string databaseID;
		std::string databaseClassName;
		if (transactionDB)
		{
			databaseID = transactionDB->ID();
			databaseClassName = transactionDB->className();
		}
		db::TdlTransactionFunctionList funclist
			= db::loadTransactionProgramFile( filename, databaseID, databaseClassName, languageDescr);

		db::TdlTransactionFunctionList::const_iterator fi = funclist.begin(), fe = funclist.end();
		for (; fi != fe; ++fi)
		{
			library.defineFormFunction( fi->first, fi->second);
			LOG_DEBUG << "Loaded transaction function '" << fi->first << "'";
		}
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string( "error loading transaction definition program '") + filename + "': " + e.what());
	}
}


