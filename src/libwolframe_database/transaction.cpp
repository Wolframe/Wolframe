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
//\file database/transaction.hpp
//\brief Implementation database transaction
#include "database/transaction.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

Transaction::Result Transaction::executeStatement( const std::string& stm, const std::vector<types::Variant>& params)
{
	Result rt;
	TransactionInput input;
	input.startCommand( 1, 0, stm, -1);
	std::vector<types::Variant>::const_iterator pi = params.begin(), pe = params.end();
	for (; pi != pe; ++pi)
	{
		input.bindCommandArgAsValue( *pi);
	}
	TransactionOutput output;
	execute( input, output);
	TransactionOutput::result_const_iterator ri = output.begin(), re = output.end();
	if (ri == re) return rt;
	if (ri +1 != re) throw std::logic_error("internal: more than one result (set of rows) for one command");
	std::vector<std::string> colnames;
	std::vector<Transaction::Result::Row> rows;

	std::size_t ci=0, ce = ri->nofColumns();
	for (; ci != ce; ++ci)
	{
		colnames.push_back( ri->columnName( ci));
	}
	std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi = ri->begin(), we = ri->end();
	for (; wi != we; ++wi)
	{
		rows.push_back( *wi);
	}
	return Result( colnames, rows);
}


