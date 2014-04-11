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
#include "vm/inputStructure.hpp"
#include "tdl2vmTranslator.hpp"
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
	const vm::Program* program;
};

static void programInstance_logTraceCallBack_TRACE( const vm::ProgramInstance::LogTraceContext* ctx, unsigned int ip)
{
	utils::FileLineInfo posinfo;
	if (ctx->program->getSourceReference( ip, posinfo))
	{
		LOG_TRACE << "[transaction vm] execute [" << ip << "]" << ctx->program->instructionStringAt( ip) << " " << posinfo.logtext_short();
	}
}

static void programInstance_logTraceCallBack_DEBUG( const vm::ProgramInstance::LogTraceContext* ctx, unsigned int ip)
{
	utils::FileLineInfo posinfo;
	if (ctx->program->getSourceReference( ip, posinfo))
	{
		LOG_DEBUG << "[transaction vm] execute [" << ip << "]" << ctx->program->instructionStringAt( ip);
	}
}


void Transaction::execute( const VmTransactionInput& input, VmTransactionOutput& output)
{
	vm::ProgramInstance::LogTraceContext context;
	context.program = &input.program();

	vm::ProgramInstance::LogTraceCallBack logtrace = 0;
	if (log::LogBackend::instance().minLogLevel() <= log::LogLevel::LOGLEVEL_DEBUG)
	{
		if (log::LogBackend::instance().minLogLevel() <= log::LogLevel::LOGLEVEL_TRACE)
		{
			logtrace = &programInstance_logTraceCallBack_TRACE;
		}
		else
		{
			logtrace = &programInstance_logTraceCallBack_DEBUG;
		}
	}
	vm::ProgramInstance instance( context.program, m_stm.get(), logtrace, &context);

	bool result;
	try
	{
		result = instance.execute();
	}
	catch (const std::runtime_error& e)
	{
		std::string locationstr;
		utils::FileLineInfo posinfo;
		if (input.program().getSourceReference( instance.ip(), posinfo))
		{
			locationstr = std::string(" TDL source location ") + posinfo.logtext();
		}
		LOG_ERROR << "exception thrown in transaction '" << m_name
				<< "' at VM IP " << instance.ip()
				<< " instruction " << input.program().instructionStringAt( instance.ip())
				<< locationstr;
		m_stm->rollback();
		throw e;
	}
	if (!result)
	{
		const DatabaseError& err = instance.lastError();
		std::string errordetail = removeCRLF( err.errordetail);
		std::string errormsg = removeCRLF( err.errormsg);
		
		std::ostringstream logmsg;
		logmsg << "error in transaction '" << m_name << "' [IP " << err.ip << "]"
			<< " for database '" << err.dbname << "'"
			<< " error class '" << err.errorclass << "'";
		if (err.errorcode)
		{
			logmsg << " database internal error code " << err.errorcode << "'";
		}
		if (err.errormsg.size())
		{
			logmsg << " " << errormsg;
		}
		if (err.errordetail.size())
		{
			logmsg << " " << errordetail;
		}
		if (err.errorhint.size())
		{
			logmsg << " " << err.errorhint;
		}
		std::string locationstr;
		utils::FileLineInfo posinfo;
		if (input.program().getSourceReference( instance.ip(), posinfo))
		{
			logmsg << " TDL source location " << posinfo.logtext();
		}
		LOG_ERROR << logmsg.str();

		std::ostringstream throwmsg;
		//... shorter message is thrown. no internals in message as written into the logs
		throwmsg << "error in transaction '" << m_name << "':";
		if (err.errordetail.size())
		{
			throwmsg << " " << errordetail;
		}
		else if (err.errormsg.size())
		{
			//... error message is only thrown if errordetail is empty
			throwmsg << " " << errormsg;
		}
		if (err.errorhint.size())
		{
			throwmsg << " " << err.errorhint;
		}
		m_stm->rollback();

		throw std::runtime_error( throwmsg.str());
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
}

static vm::ProgramR singleStatementProgram( const std::string& stm, const std::vector<types::Variant>& params)
{
	std::vector<std::string> resultpath;
	resultpath.push_back( "");
	types::keymap<vm::Subroutine> sm;

	Tdl2vmTranslator prg( &sm, false);
	prg.begin_DO_statement( stm);
	std::vector<types::Variant>::const_iterator pi = params.begin(), pe = params.end();
	for (; pi != pe; ++pi)
	{
		prg.push_ARGUMENT_CONST( *pi);
	}
	prg.end_DO_statement();
	prg.begin_loop_INTO_block( resultpath);
	prg.output_statement_result( true);
	prg.end_loop_INTO_block();
	return prg.createProgram();
}

Transaction::Result Transaction::executeStatement( const std::string& stm, const std::vector<types::Variant>& params)
{
	vm::ProgramR program = singleStatementProgram( stm, params);

	VmTransactionInput input( *program, vm::InputStructure( program->pathset.tagtab()));
	VmTransactionOutput output;
	execute( input, output);

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
			throw std::runtime_error( "internal: unexpected element after final CLOSE");
		}
		switch (type)
		{
			case langbind::FilterBase::OpenTag:
				taglevel++;
				if (taglevel == 1)
				{
					if (value.type() != types::Variant::String || value.charsize() != 0)
					{
						throw std::runtime_error( "internal: unexpected top level tag in result");
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
					throw std::runtime_error( "internal: unexpected structure in result");
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
						throw std::runtime_error("internal: row size does not mach to number of columns");
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
					throw std::runtime_error( "internal: unexpected attribute in result");
				}
				break;
			case langbind::FilterBase::Value:
				row.push_back( value);
				break;
		}
	}
	return Result( colnames, rows);
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



