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
///\brief Implements the main execution loop (non database specific part) of the standard database transaction execution statemechine
///\file transactionExecStatemachine.cpp
#include "database/transactionExecStatemachine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

#define ERRORCODE(E) (E+10000)

static std::size_t resolveColumnName( const TransactionOutput::CommandResult& cmdres, const std::string& colname)
{
	std::size_t ci = 0, ce = cmdres.nofColumns();
	for (; ci != ce; ++ci)
	{
		if (boost::algorithm::iequals( cmdres.columnName( ci), colname))
		{
			return ci;
		}
	}
	throw std::runtime_error( std::string( "column name '") + colname + "' not found in result");
}

static types::VariantConst resolveResultReference( const TransactionOutput& output, const TransactionInput::cmd_const_iterator& cmditr, std::size_t result_functionidx, const types::Variant& reference)
{
	types::VariantConst rt;
	TransactionOutput::result_const_iterator fi = output.end();
	std::size_t prev_functionidx = std::numeric_limits<std::size_t>::max();
	bool found = false;

	TransactionOutput::result_const_iterator ri = output.begin() + output.size();
	while (ri != output.begin())
	{
		-- ri;
		if (prev_functionidx < ri->functionidx())
		{
			break; //... crossing operation scope border
		}
		prev_functionidx = ri->functionidx();
		if (result_functionidx >= ri->functionidx())
		{
			fi = ri;
			found = true;
			break;
		}
	}
	if (found)
	{
		std::size_t cidx;
		if (reference.type() == types::Variant::String)
		{
			cidx = resolveColumnName( *fi, reference.tostring());
		}
		else
		{
			cidx = reference.touint();
			if (cidx == 0)
			{
				db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(41), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "result reference out of range. column name must be >= 1", "internal logic error (transaction function variable reference)");
				throw db::DatabaseErrorException( dberr);
			}
			if (cidx > fi->nofColumns())
			{
				db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(42), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "result reference out of range. array bound read", "internal logic error (transaction function variable reference)");
				throw db::DatabaseErrorException( dberr);
			}
			cidx -= 1;
		}
		if (fi->nofRows() > 1)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(43), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "unbound result reference to set of results", "internal logic error (transaction function variable reference)");
			throw db::DatabaseErrorException( dberr);
		}
		if (fi->nofRows() > 0)
		{
			rt = fi->begin()->at( cidx);
		}
	}
	return rt;
}

static types::VariantConst resolveResultIteratorReference( const TransactionOutput::CommandResult& cmdres, const std::vector<TransactionOutput::CommandResult::Row>::const_iterator& resrow, const types::Variant& reference, const TransactionInput::cmd_const_iterator& cmditr)
{
	types::VariantConst rt;
	std::size_t cidx;
	if (reference.type() == types::Variant::String)
	{
		cidx = resolveColumnName( cmdres, reference.tostring());
	}
	else
	{
		cidx = reference.touint();
		if (cidx == 0)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(21), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "result reference out of range. must be >= 1", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
		if (cidx > resrow->size())
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(22), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "result reference out of range. array bound read", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
		cidx -= 1;
	}
	rt = resrow->at( cidx);
	return rt;
}

static bool executeCommand( TransactionExecStatemachine* stmh, const TransactionOutput& output, TransactionOutput::CommandResult& cmdres, std::size_t residx, std::size_t rowidx, const TransactionInput::cmd_const_iterator& cmditr, bool nonempty, bool unique)
{
	/*[-]*/LOG_DATA << "CALL executeCommand " << cmditr->tostring();
	TransactionInput::Command::arg_const_iterator ai = cmditr->arg().begin(), ae = cmditr->arg().end();
	for (int argidx=1; ai != ae; ++ai,++argidx)
	{
		types::VariantConst val;
		switch (ai->type())
		{
			case TransactionInput::Command::Argument::ResultColumn:
			{
				if (ai->scope_functionidx() == 0)
				{
					//... parameter reference
					val = resolveResultReference( output, cmditr, ai->scope_functionidx(), ai->value());
				}
				else if (ai->scope_functionidx() > 0)
				{
					val = resolveResultReference( output, cmditr, ai->scope_functionidx()-1, ai->value());
				}
				else if (cmditr->foreach_functionidx() == -1)
				{
					if (cmditr->functionidx() > 0)
					{
						//... result reference without namespace in unbound command
						val = resolveResultReference( output, cmditr, cmditr->functionidx()-1, ai->value());
					}
					else
					{
						db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(31), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "Referencing result in first command", "semantic error");
						throw db::DatabaseErrorException( dberr);
					}
				}
				else if (residx < output.size())
				{
					TransactionOutput::result_const_iterator resitr = output.begin() + residx;
					std::vector<TransactionOutput::CommandResult::Row>::const_iterator rowitr = resitr->begin() + rowidx;
					val = resolveResultIteratorReference( cmdres, rowitr, ai->value(), cmditr);
				}
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
				db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(1), 0/*dbname*/, cmditr->statement().c_str(), "NOTUNIQUE", "more than one result result for command (UNIQUE)", "internal data constraint validation error");
				throw db::DatabaseErrorException( dberr);
			}
		}
		else if (nonempty)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(2), 0/*dbname*/, cmditr->statement().c_str(), "NOTNONEMPTY", "missing result for command (NONEMPTY)", "internal data constraint validation error");
			throw db::DatabaseErrorException( dberr);
		}
	}
	else
	{
		if (nonempty)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(11), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "unexpected condition NONEMPTY on result for a command without result set", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
		if (unique)
		{
			db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(12), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "unexpected condition UNIQUE on result for a command without result set", "internal logic error (transaction function definition)");
			throw db::DatabaseErrorException( dberr);
		}
	}
	return true;
}

static bool pushArguments( const TransactionOutput& output, TransactionOutput::CommandResult& cmdres, std::size_t residx, std::size_t rowidx, const TransactionInput::cmd_const_iterator& cmditr)
{
	TransactionInput::Command::arg_const_iterator ai = cmditr->begin(), ae = cmditr->end();
	int argidx;
	if (!cmdres.nofColumns())
	{
		for (argidx=1; ai != ae; ++ai,++argidx)
		{
			std::ostringstream colname;
			colname << "_" << argidx;
			cmdres.addColumn( colname.str());
		}
	}
	cmdres.openRow();
	for (argidx=1,ai=cmditr->begin(); ai != ae; ++ai,++argidx)
	{
		types::VariantConst val;

		switch (ai->type())
		{
			case TransactionInput::Command::Argument::ResultColumn:
			{
				if (ai->scope_functionidx() == 0)
				{
					//... parameter reference
					val = resolveResultReference( output, cmditr, ai->scope_functionidx(), ai->value());
				}
				else if (ai->scope_functionidx() > 0)
				{
					val = resolveResultReference( output, cmditr, ai->scope_functionidx()-1, ai->value());
				}
				else if (cmditr->foreach_functionidx() == -1)
				{
					if (cmditr->functionidx() > 0)
					{
						//... result reference without namespace in unbound command
						val = resolveResultReference( output, cmditr, cmditr->functionidx()-1, ai->value());
					}
					else
					{
						db::DatabaseError dberr( _Wolframe::log::LogLevel::LOGLEVEL_ERROR, ERRORCODE(31), 0/*dbname*/, cmditr->statement().c_str(), "INTERNAL", "Referencing result in first command", "semantic error");
						throw db::DatabaseErrorException( dberr);
					}
				}
				else if (residx < output.size())
				{
					TransactionOutput::result_const_iterator resitr = output.begin() + residx;
					std::vector<TransactionOutput::CommandResult::Row>::const_iterator rowitr = resitr->begin() + rowidx;
					val = resolveResultIteratorReference( cmdres, rowitr, ai->value(), cmditr);
				}
				break;
			}
			case TransactionInput::Command::Argument::Value:
				val = ai->value();
				break;
		}
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
	for (++ci; ci != ce && (ci->level() > level || (ci->level() == level && !ci->statement().empty())); ++ci);
	return ci;
}

namespace {
struct OperationLoop
{
	std::size_t residx;
	std::size_t rowidx;
	TransactionInput::cmd_const_iterator ci,ce;

	OperationLoop( TransactionInput::cmd_const_iterator ci_, TransactionInput::cmd_const_iterator ce_, std::size_t residx_, std::size_t rowidx_)
		:residx(residx_),rowidx(rowidx_),ci(ci_),ce(ce_){}
	OperationLoop( const OperationLoop& o)
		:residx(o.residx),rowidx(o.rowidx),ci(o.ci),ce(o.ce){}
};
}//namespace


bool TransactionExecStatemachine::doTransaction( const TransactionInput& input, TransactionOutput& output)
{
	LOG_DATA << "[execute transaction] " << input.tostring();
	enum OperationType
	{
		DatabaseCall,
		PushArguments
	};
	OperationType optype = DatabaseCall;
	std::size_t null_functionidx = std::numeric_limits<std::size_t>::max();
	TransactionOutput::CommandResult cmdres( null_functionidx);
	std::vector<OperationLoop> loopstk;

	TransactionInput::cmd_const_iterator ci = input.begin(), ce = input.end();
	while (ci != ce)
	{
		if (ci->statement().empty())
		{
			optype = PushArguments;
		}
		else
		{
			optype = DatabaseCall;
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
			cmdres = TransactionOutput::CommandResult( ci->functionidx());
		}
		if (ci->foreach_functionidx() > 0)
		{
			// ... command is bound to a result set, so we call it for every result row
			TransactionOutput::result_const_iterator ri;
			switch (optype)
			{
				case PushArguments:
					// start of an operation: execution of a instruction block
					ri = output.resultIterator( ci->foreach_functionidx()-1);
					if (ri != output.end())
					{
						std::size_t residx = ri - output.begin(); //< start of result referenced by pushArguments
						if (ri->nofRows())
						{
							LOG_DATA << "start of loop on result for calling an OPERATION";
							// ... result non empty, then push arguments of first row
							if (!pushArguments( output, cmdres, residx, 0/*rowidx*/, ci)) return false;
							// ... put on the loop stack the following row
							loopstk.push_back(
								OperationLoop( ci,//...push parameters instruction of operation
										endOfOperation( ci, ce),//...next instruction after operation
										residx, 1//...next row index after first
									));
							break;
						}
						// ... result non empty = no result
					}
					//... no result, so we have to skip all operation steps, not only the parameter passing
					ci = endOfOperation( ci, ce);
					--ci; //... decrement for loop increment compensate
					break;

				case DatabaseCall:
					ri = output.resultIterator( ci->foreach_functionidx()-1);
					if (ri != output.end())
					{
						std::size_t residx = ri - output.begin();
						std::vector<TransactionOutput::CommandResult::Row>::const_iterator wi = ri->begin(), we = ri->end();
						for (std::size_t rowidx=0; wi != we; ++wi,++rowidx)
						{
							if (!start( ci->statement())
							||  !executeCommand( this, output, cmdres, residx, rowidx, ci, nonempty, unique))
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
			// ... command is not bound to a result set, then we call it once
			std::size_t residx = output.size();
			switch (optype)
			{
				case PushArguments:
					if (!pushArguments( output, cmdres, residx, 0/*rowidx*/, ci)) return false;
					break;
				case DatabaseCall:
					if (!start( ci->statement())
					||  !executeCommand( this, output, cmdres, residx, 0/*rowidx*/, ci, nonempty, unique))
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

		// Fetch next instrucution:
		++ci;
		if (!loopstk.empty())
		{
			if (ci == loopstk.back().ce)
			{
				// ... we reached the end of a loop on result executing an operation sub block
				std::size_t residx = loopstk.back().residx;
				std::size_t rowidx = loopstk.back().rowidx++; // ... skip to the next row of the result to process by the operation
				if (rowidx < output.at(residx).nofRows())
				{
					// ... there are still results left to process
					LOG_DATA << "skip to element " << (rowidx+1) << " of loop on result for calling an OPERATION";
					ci = loopstk.back().ci; //... jump back to parameter pass instruction of the operation
					if (cmdres.functionidx() != null_functionidx && cmdres.nofColumns())
					{
						output.addCommandResult( cmdres);//... add command result
					}
					cmdres = TransactionOutput::CommandResult( ci->functionidx());
					if (!pushArguments( output, cmdres, residx, rowidx, ci)) return false;
					++ci;//... skip to first instruction of the operation
				}
				else
				{
					LOG_DATA << "end of loop on result for calling an OPERATION";
					loopstk.pop_back();
				}
			}
		}

	}
	if (cmdres.functionidx() != null_functionidx)
	{
		output.addCommandResult( cmdres);
	}
	output.setCaseSensitive( isCaseSensitive());
	return true;
}


