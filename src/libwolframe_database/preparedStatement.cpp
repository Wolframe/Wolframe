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
///\brief Implements execution of a transaction using a prepared statement interface
///\file preparedStatement.cpp
#include "database/preparedStatement.hpp"
#include <iostream>
#include <fstream>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::db;

static bool executeCommand( PreparedStatementHandler* stmh, TransactionOutput::CommandResultBuilder& cmdres, const TransactionOutput::row_iterator& resrow, const TransactionInput::cmd_iterator& cmditr, bool nonempty, bool unique)
{
	TransactionInput::arg_iterator ai = cmditr->begin(), ae = cmditr->end();
	for (int argidx=1; ai != ae; ++ai,++argidx)
	{
		const char* val = 0;
		switch (ai->type())
		{
			case TransactionInput::Element::ResultColumn:
				if (ai->ref() == 0) throw std::runtime_error( "result reference out of range. must be >= 1");
				if (ai->ref() > resrow->size()) throw std::runtime_error( "result reference out of range. array bound read");
				val = (*resrow)[ ai->ref() -1];
				break;

			case TransactionInput::Element::String:
				val = ai->value();
				break;
		}
		if (!stmh->bind( argidx, val)) return false;
	}
	if (!stmh->execute()) return false;

	unsigned int si, se = stmh->nofColumns();
	if (se)
	{
		if (!cmdres.nofColumns())
		{
			for (si=0; si != se; ++si)
			{
				const char* colname = stmh->columnName( si+1);
				cmdres.addColumn( colname?colname:"");
			}
		}
		if (stmh->hasResult())
		{
			std::size_t rescnt = 0;
			do
			{
				rescnt += 1;
				cmdres.openRow();
				for (si=0; si != se; ++si)
				{
					const char* col = stmh->get( si+1);
					if (col)
					{
						cmdres.addValue( col);
					}
					else
					{
						cmdres.addNull();
					}
				}
			} while (stmh->next());
			if (unique && rescnt > 1)
			{
				throw std::runtime_error( std::string( "more than one result result for command (UNIQUE) ") + cmditr->name());
			}
		}
		else if (nonempty)
		{
			throw std::runtime_error( std::string( "missing result for command (NONEMPTY) ") + cmditr->name());
		}
	}
	else
	{
		if (nonempty)
		{
			throw std::runtime_error( std::string( "unexpected condition NONEMPTY on result for a command without result set: ") + cmditr->name());
		}
		if (unique)
		{
			throw std::runtime_error( std::string( "unexpected condition UNIQUE on result for a command without result set: ") + cmditr->name());
		}
	}
	return true;
}

bool PreparedStatementHandler::doTransaction( const TransactionInput& input, TransactionOutput& output)
{
	std::size_t null_functionidx = std::numeric_limits<std::size_t>::max();
	TransactionOutput::CommandResultBuilder cmdres( &output, null_functionidx);

	TransactionInput::cmd_iterator ci = input.begin(), ce = input.end();
	for (; ci != ce; ++ci)
	{

		if (!start( ci->name())) return false;
		bool nonempty = input.hasNonemptyResult( ci->functionidx());
		bool unique = input.hasUniqueResult( ci->functionidx());

		if (cmdres.functionidx() != ci->functionidx())
		{
			// When we get to the next group of functions with the same id
			// we add the result if there exist one of the previous
			// function group as last result (to be referenced by $1,$2,etc.).
			if (cmdres.functionidx() != null_functionidx && cmdres.nofColumns())
			{
				output.addCommandResult( cmdres);
			}
			cmdres = TransactionOutput::CommandResultBuilder( &output, ci->functionidx());
		}
		TransactionInput::arg_iterator ai = ci->begin(), ae = ci->end();

		for (; ai != ae && ai->type() != TransactionInput::Element::ResultColumn; ++ai);
		if (ai != ae)
		{
			// ... command has result reference, then we call it for every result row
			TransactionOutput::result_iterator ri = output.last();
			if (ri != output.end())
			{
				TransactionOutput::row_iterator wi = ri->begin(), we = ri->end();
				for (; wi != we; ++wi)
				{
					if (!executeCommand( this, cmdres, wi, ci, nonempty, unique)) return false;
				}
			}
		}
		else
		{
			// ... command has no result reference, then we call it once
			TransactionOutput::row_iterator wi;
			if (!executeCommand( this, cmdres, wi, ci, nonempty, unique)) return false;
		}
	}
	if (cmdres.functionidx() != null_functionidx)
	{
		output.addCommandResult( cmdres);
	}
	return true;
}


