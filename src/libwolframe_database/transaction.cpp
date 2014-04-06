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
#include "transactionfunction/InputStructure.hpp"
#include "tdl2vmTranslator.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

void Transaction::execute( const VmTransactionInput& input, VmTransactionOutput& output)
{
	vm::ProgramInstance instance( &input.program(), m_stm.get());
	if (!instance.execute())
	{
		throw std::runtime_error("unexpected transaction vm program termination");
	}
	output = VmTransactionOutput( instance.output());
}

static vm::ProgramR singleStatementProgram( const std::string& stm, const std::vector<types::Variant>& params)
{
	std::vector<std::string> resultpath;
	resultpath.push_back( "");
	types::keymap<vm::Subroutine> sm;

	Tdl2vmTranslator prg( &sm);
	prg.begin_DO_statement( stm);
	std::vector<types::Variant>::const_iterator pi = params.begin(), pe = params.end();
	for (; pi != pe; ++pi)
	{
		prg.push_ARGUMENT_CONST( *pi);
	}
	prg.end_DO_statement();
	prg.output_statement_result( resultpath);
	return prg.createProgram();
}

Transaction::Result Transaction::executeStatement( const std::string& stm, const std::vector<types::Variant>& params)
{
	vm::ProgramR program = singleStatementProgram( stm, params);

	VmTransactionInput input( *program, tf::InputStructure( program->pathset.tagtab()));
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


