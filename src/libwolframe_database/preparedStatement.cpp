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
///\brief Implements execution of a transaction using a prepared statement interface
///\file preparedStatement.cpp
#include "database/preparedStatement.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::db;

#define ERRORCODE(E) (E+0x1000000)

static bool executeCommand( PreparedStatementHandler* stmh, TransactionOutput::CommandResult& cmdres, const std::vector<TransactionOutput::CommandResult::Row>::const_iterator& resrow, const TransactionInput::cmd_const_iterator& cmditr, bool nonempty, bool unique)
{
	TransactionInput::Command::arg_const_iterator ai = cmditr->arg().begin(), ae = cmditr->arg().end();
	for (int argidx=1; ai != ae; ++ai,++argidx)
	{
		types::VariantConst val;

		switch (ai->type())
		{
			case TransactionInput::Command::Argument::ResultColumn:
			{
				unsigned int ref = ai->value().touint();
				if (ref == 0)
				{
					db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(21), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "result reference out of range. must be >= 1", "internal logic error (transaction function definition)");
					throw db::DatabaseErrorException( dberr);
				}
				if (ref > resrow->size())
				{
					db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(22), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "result reference out of range. array bound read", "internal logic error (transaction function definition)");
					throw db::DatabaseErrorException( dberr);
				}
				val = resrow->at( ref - 1);
				break;
			}
			case TransactionInput::Command::Argument::Value:
				val = ai->value();
				break;
		}
		if (!stmh->bind( argidx, val))
		{
			if (stmh->getLastError()) throw db::DatabaseErrorException( *stmh->getLastError());
		}
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
				if (colname)
				{
					if (colname[0] == '_' && colname[1] == '\0')
					{
						cmdres.addColumn( "");
					}
					else
					{
						cmdres.addColumn( colname);
					}
				}
				else
				{
					cmdres.addColumn( "");
				}
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
					types::VariantConst col = stmh->get( si+1);
					if (col.defined())
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
				db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(1), 0/*dbname*/, cmditr->name().c_str(), "NOTUNIQUE", "more than one result result for command (UNIQUE)", "internal data constraint validation error");
				throw db::DatabaseErrorException( dberr);
			}
		}
		else if (nonempty)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(2), 0/*dbname*/, cmditr->name().c_str(), "NOTNONEMPTY", "missing result for command (NONEMPTY)", "internal data constraint validation error");
			throw db::DatabaseErrorException( dberr);
		}
	}
	else
	{
		if (nonempty)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(11), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "unexpected condition NONEMPTY on result for a command without result set", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
		if (unique)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(12), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "unexpected condition UNIQUE on result for a command without result set", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
	}
	return true;
}

static bool pushArguments( TransactionOutput::CommandResult& cmdres, const std::vector<TransactionOutput::CommandResult::Row>::const_iterator& resrow, const TransactionInput::cmd_const_iterator& cmditr)
{
	TransactionInput::Command::arg_const_iterator ai = cmditr->begin(), ae = cmditr->end();
	cmdres.openRow();
	for (int argidx=1; ai != ae; ++ai,++argidx)
	{
		types::VariantConst val;

		switch (ai->type())
		{
			case TransactionInput::Command::Argument::ResultColumn:
			{
				unsigned int ref = ai->value().touint();
				if (ref == 0)
				{
					db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(21), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "result reference out of range. must be >= 1", "internal logic error (transaction function definition)");
					throw db::DatabaseErrorException( dberr);
				}
				if (ref > resrow->size())
				{
					db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(22), 0/*dbname*/, cmditr->name().c_str(), "INTERNAL", "result reference out of range. array bound read", "internal logic error (transaction function definition)");
					throw db::DatabaseErrorException( dberr);
				}
				val = resrow->at( ref - 1);
				break;
			}
			case TransactionInput::Command::Argument::Value:
				val = ai->value();
				break;
		}
		std::ostringstream colname;
		colname << "_" << argidx;
		cmdres.addColumn( colname.str());

		if (val.defined())
		{
			cmdres.addValue( val);
		}
		else
		{
			cmdres.addNull();
		}
	}
	return true;
}

static TransactionInput::cmd_const_iterator endOfOperation( TransactionInput::cmd_const_iterator ci, TransactionInput::cmd_const_iterator ce)
{
	std::size_t level = ci->level();
	for (++ci; ci != ce && (ci->level() > level || (ci->level() == level && !ci->name().empty())); ++ci);
	return ci;
}

namespace {
struct OperationLoop
{
	std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi,we;
	TransactionInput::cmd_const_iterator ci,ce;

	OperationLoop( TransactionInput::cmd_const_iterator ci_, TransactionInput::cmd_const_iterator ce_, std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi_, std::vector<TransactionOutput::CommandResult::Row>::const_iterator we_)
		:wi(wi_),we(we_),ci(ci_),ce(ce_){}
	OperationLoop( const OperationLoop& o)
		:wi(o.wi),we(o.we),ci(o.ci),ce(o.ce){}
};
}//namespace

bool PreparedStatementHandler::doTransaction( const TransactionInput& input, TransactionOutput& output)
{
	enum OperationType
	{
		DatabaseCall,
		PushArguments
	};
	OperationType optype = DatabaseCall;
	std::size_t null_functionidx = std::numeric_limits<std::size_t>::max();
	TransactionOutput::CommandResult cmdres( null_functionidx, 0);
	std::vector<OperationLoop> loopstk;

	TransactionInput::cmd_const_iterator ci = input.begin(), ce = input.end();

	for (; ci != ce; ++ci)
	{
		if (!loopstk.empty())
		{
			if (ci == loopstk.back().ce)
			{
				if (loopstk.back().wi != loopstk.back().we)
				{
					ci = loopstk.back().ci;
					if (!pushArguments( cmdres, loopstk.back().wi, ci)) return false;
					++loopstk.back().wi;
				}
				else
				{
					loopstk.pop_back();
				}
			}
		}
		if (ci->name().empty())
		{
			optype = PushArguments;
		}
		else
		{
			optype = DatabaseCall;
			if (!start( ci->name())) return false;
		}
		bool nonempty = ci->nonemptyResult();
		bool unique = ci->uniqueResult();

		if (cmdres.functionidx() != ci->functionidx())
		{
			// When we get to the next group of functions with the same id
			// we add the result if there exist one of the previous
			// function group as last result (to be referenced by $1,$2,etc.).
			if (cmdres.functionidx() != null_functionidx && cmdres.nofColumns())
			{
				output.addCommandResult( cmdres);
			}
			cmdres = TransactionOutput::CommandResult( ci->functionidx(), ci->level());
		}
		TransactionInput::Command::arg_const_iterator ai = ci->begin(), ae = ci->end();

		for (; ai != ae && ai->type() != TransactionInput::Command::Argument::ResultColumn; ++ai);
		if (ai != ae)
		{
			// ... command has result reference, then we call it for every result row
			TransactionOutput::result_const_iterator ri;
			switch (optype)
			{
				case PushArguments:
					// start of an operation: execution of a instruction block
					ri = output.last( ci->level()-1);
					if (ri != output.end())
					{
						std::vector<TransactionOutput::CommandResult::Row>::const_iterator
							wi = ri->begin(), we = ri->end();
						if (wi != we)
						{
							if (!pushArguments( cmdres, wi, ci)) return false;
							loopstk.push_back( OperationLoop( ci, endOfOperation( ci, ce), ++wi, we));
							break;
						}
					}
					//... no result, so we have to skip all operation steps, not only the parameter passing
					ci = endOfOperation( ci, ce);
					--ci; //... decrement for loop increment compensate
					break;

				case DatabaseCall:
					ri = output.last( ci->level());
					if (ri != output.end())
					{
						std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi = ri->begin(), we = ri->end();
						for (; wi != we; ++wi)
						{
							if (!executeCommand( this, cmdres, wi, ci, nonempty, unique))
							{
								const DatabaseError* lasterr = getLastError();
								if (lasterr)
								{
									DatabaseError err( *lasterr);
									err.functionidx = ci->functionidx();
									throw DatabaseErrorException( err);
								}
								return false;
							}
						}
					}
			}
		}
		else
		{
			// ... command has no result reference, then we call it once
			std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi;
			switch (optype)
			{
				case PushArguments:
					if (!pushArguments( cmdres, wi, ci)) return false;
					break;
				case DatabaseCall:
					if (!executeCommand( this, cmdres, wi, ci, nonempty, unique))
					{
						const DatabaseError* lasterr = getLastError();
						if (lasterr)
						{
							DatabaseError err( *lasterr);
							err.functionidx = ci->functionidx();
							throw DatabaseErrorException( err);
						}
						return false;
					}
					break;
			}
		}
	}
	if (cmdres.functionidx() != null_functionidx)
	{
		output.addCommandResult( cmdres);
	}
	output.setCaseSensitive( isCaseSensitive());
	LOG_DATA << "transaction function output structure:" << output.tostring();
	return true;
}


