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
///\brief Loading function for transaction definition programs
///\file database/loadTransactionProgram.hpp
#ifndef _DATABASE_LOAD_TRANSACTION_PROGRAM_HPP_INCLUDED
#define _DATABASE_LOAD_TRANSACTION_PROGRAM_HPP_INCLUDED
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

std::vector<std::pair<std::string,TransactionFunctionR> >
	loadTransactionProgramFile( const std::string& filename, const LanguageDescription* langdescr, std::string& dbsource, types::keymap<std::string>& embeddedStatementMap);


}}//namespace
#endif

