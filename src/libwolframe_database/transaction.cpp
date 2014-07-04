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
//\file database/transaction.hpp
//\brief Implementation database transaction
#include "database/transaction.hpp"
#include "database/vm/programInstance.hpp"
#include "database/vm/instructionSet.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

static std::string removeCRLF( const std::string& src)
{
	std::string rt;
	std::string::const_iterator si = src.begin(), se = src.end();
	for (; si != se; ++si)
	{
		if (*si == '\r' || *si == '\n')
		{
			if (rt.size() && rt[ rt.size()-1] != ' ') rt.push_back(' ');
		}
		else
		{
			rt.push_back( *si);
		}
	}
	return rt;
}

struct vm::ProgramInstance::LogTraceContext
{
	const vm::ProgramImage* program;
};

static void programInstance_logTraceCallBack_TRACE( const vm::ProgramInstance::LogTraceContext* ctx, unsigned int ip)
{
	std::ostringstream str;
	ctx->program->printInstruction( str, ctx->program->code[ ip]);
	LOG_TRACE << "[transaction vm] execute [" << ip << "] " << str.str();
}

bool Transaction::execute( const VmTransactionInput& input, VmTransactionOutput& output)
{
	vm::ProgramInstance::LogTraceContext context;
	context.program = &input.program();

	vm::ProgramInstance::LogTraceCallBack logtrace = 0;
	if (log::LogBackend::instance().minLogLevel() <= log::LogLevel::LOGLEVEL_TRACE)
	{
		logtrace = &programInstance_logTraceCallBack_TRACE;
	}
	vm::ProgramInstance instance( *context.program, m_stm.get(), logtrace, &context);
	try
	{
		if (!instance.execute())
		{
			const DatabaseError* err = instance.lastError();
			if (err)
			{
				m_lastError = *err;
			}
			else
			{
				m_lastError = DatabaseError( "UNKNOWN", 0, "unknown error");
			}
			m_stm->rollback();
			return false;
		}
	}
	catch (const std::runtime_error& e)
	{
		m_lastError = DatabaseError( "EXCEPTION", 0, e.what());
		m_lastError.ip = instance.ip();
		m_stm->rollback();
		return false;
	}
	catch (const std::exception& e)
	{
		m_stm->rollback();
		throw e;
	}
	output = VmTransactionOutput( instance.output());
	if (log::LogBackend::instance().minLogLevel() <= log::LogLevel::LOGLEVEL_DATA)
	{
		langbind::TypedInputFilterR resultfilter = output.get();
		langbind::FilterBase::ElementType elemtype;
		types::VariantConst elemvalue;
		while (resultfilter->getNext( elemtype, elemvalue))
		{
			if (elemvalue.defined())
			{
				LOG_DATA << "[transaction output] element " << langbind::FilterBase::elementTypeName( elemtype) << " '" << elemvalue.tostring() << "' :" << elemvalue.typeName();
			}
			else
			{
				LOG_DATA << "[transaction output] element " << langbind::FilterBase::elementTypeName( elemtype);
			}
		}
	}
	return true;
}

static std::string errorMessageString( const DatabaseError& err)
{
	std::string errordetail = removeCRLF( err.errordetail);
	std::string errormsg = removeCRLF( err.errormsg);
	
	std::ostringstream logmsg;
	logmsg << "error in database '" << err.dbname << "'"
		<< " error class '" << err.errorclass << "'";

	if (err.errorcode)
	{
		logmsg << " database internal error code " << err.errorcode << "'";
	}
	if (errormsg.size())
	{
		logmsg << " " << errormsg;
	}
	if (errordetail.size())
	{
		logmsg << " " << errordetail;
	}
	return logmsg.str();
}

static vm::ProgramImage singleStatementProgram( const std::string& stm, const std::vector<types::Variant>& params)
{
	vm::ProgramImage rt;

	typedef vm::InstructionSet IS;
	//Tdl2vmTranslator prg( &sm, false);
	//prg.begin_DO_statement( stm);
	rt.code
		( IS::Op_RESULT_SET_INIT )
		( IS::Op_DBSTM_START, 0 )
	;
	rt.statements.push_back( stm);

	std::vector<types::Variant>::const_iterator pi = params.begin(), pe = params.end();
	for (; pi != pe; ++pi)
	{
		//prg.push_ARGUMENT_CONST( *pi);
		rt.code
			( IS::Op_DBSTM_BIND_CONST, rt.constants.size() )
		;
		rt.constants.push_back( *pi);
	}
	//prg.end_DO_statement();
	rt.code
		( IS::Op_DBSTM_EXEC, 0 )
	;
	//prg.begin_loop_INTO_block( resultpath);
	rt.tagnames.push_back("");
	rt.code
		( IS::Op_OUTPUT_OPEN_ARRAY, 0 )
	;
	//prg.output_statement_result( true);
	IS::ArgumentIndex endofblock
		= (IS::ArgumentIndex)(rt.code.size() + 7);

	IS::ArgumentIndex startofblock
		= (IS::ArgumentIndex)(rt.code.size() + 2);

	rt.code
		( IS::Op_OPEN_ITER_LAST_RESULT )
		( IS::Co_NOT_IF_COND, IS::Op_GOTO, endofblock)
		( IS::Op_OUTPUT_OPEN_ELEM )
		( IS::Op_OUTPUT_ITR_COLUMN )
		( IS::Op_OUTPUT_CLOSE_ELEM )
		( IS::Op_NEXT )
		( IS::Co_IF_COND, IS::Op_GOTO, startofblock )
	;

	//prg.end_loop_INTO_block();
	rt.code
		( IS::Op_OUTPUT_CLOSE_ARRAY )
	;
	return rt;
}

bool Transaction::executeStatement( const std::string& stm, const std::vector<types::Variant>& params)
{
	VmTransactionInput input( singleStatementProgram( stm, params));
	VmTransactionOutput output;

	return execute( input, output);
}

bool Transaction::executeStatement( Result& result, const std::string& stm, const std::vector<types::Variant>& params)
{
	VmTransactionInput input( singleStatementProgram( stm, params));
	VmTransactionOutput output;

	if (!execute( input, output))
	{
		return false;
	}
	langbind::TypedInputFilterR of( output.get());

	bool firstRow = true;
	langbind::FilterBase::ElementType type;
	types::VariantConst value;
	int taglevel = 0;
	std::vector<std::string> colnames;
	std::vector<Result::Row> rows;
	Result::Row row;
	bool done = false;

	while (of->getNext( type, value))
	{
		if (done)
		{
			m_lastError = DatabaseError( "EXCEPTION", 0, "unexpected element after final CLOSE");
			return false;
		}
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				taglevel++;
				if (taglevel == 1)
				{
					if (value.type() != types::Variant::String || value.charsize() != 0)
					{
						m_lastError = DatabaseError( "EXCEPTION", 0, "unexpected top level tag in result");
						return false;
					}
				}
				else if (taglevel == 2)
				{
					if (firstRow)
					{
						colnames.push_back( value.tostring());
					}
				}
				else
				{
					m_lastError = DatabaseError( "EXCEPTION", 0, "unexpected structure in result");
					return false;
				}
				break;
			case langbind::FilterBase::CloseTag:
				--taglevel;
				if (taglevel == -1) done = true;
				if (taglevel == 0)
				{
					firstRow = false;
					if (row.size() != colnames.size())
					{
						m_lastError = DatabaseError( "EXCEPTION", 0, "row size does not mach to number of columns");
						return false;
					}
					rows.push_back( row);
					row.clear();
				}
				break;
			case langbind::FilterBase::Attribute:
				if (taglevel == 1)
				{
					if (firstRow)
					{
						colnames.push_back( value.tostring());
					}
				}
				else
				{
					m_lastError = DatabaseError( "EXCEPTION", 0, "unexpected attribute in result");
					return false;
				}
				break;
			case langbind::FilterBase::Value:
				row.push_back( value);
				break;
		}
	}
	result = Result( colnames, rows);
	return true;
}

void Transaction::begin()
{
	if (!m_stm->begin())
	{
		const DatabaseError* err = m_stm->getLastError();
		if (err) throw std::runtime_error( errorMessageString( *err));
	}
}

void Transaction::commit()
{
	if (!m_stm->commit())
	{
		const DatabaseError* err = m_stm->getLastError();
		if (err) throw std::runtime_error( errorMessageString( *err));
	}
}

void Transaction::rollback()
{
	if (!m_stm->rollback())
	{
		const DatabaseError* err = m_stm->getLastError();
		if (err) throw std::runtime_error( errorMessageString( *err));
	}
}



