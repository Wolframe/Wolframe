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
///\file vm/programImage.cpp
///\brief Implementation of methods of the program executing database transactions
#include "database/vm/programImage.hpp"
#include "vm/program.hpp"
#include <sstream>
#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

void ProgramImage::print( std::ostream& out) const
{
	out << "Code:" << std::endl;
	std::size_t ip = 0;
	for (; ip < code.size(); ++ip)
	{
		out << "[" << ip << "] ";
		printInstruction( out, code[ ip]);
		out << std::endl;
	}

	std::vector<ValueTupleSetR>::const_iterator vi = tuplesets.begin(), ve = tuplesets.end();
	if (vi != ve)
	{
		out << "Input Data:" << std::endl;
		for (std::size_t idx=0; vi != ve; ++vi,++idx)
		{
			out << "SET " << idx << ": ";
			std::size_t ii=1,nn = (*vi)->nofColumns();
			for (; ii<=nn;++ii)
			{
				if (ii>1) out << ", ";
				std::string colnam = (*vi)->columnName( ii);
				if (colnam.empty())
				{
					out << "<no name>";
				}
				else
				{
					out << colnam;
				}
			}
			out << std::endl;
			ValueTupleSet::const_iterator si = (*vi)->begin(), se = (*vi)->end();
			for (; si != se; ++si)
			{
				out << "  ";
				for (std::size_t colidx=1; colidx <= nn; ++colidx)
				{
					if (colidx>1) out << ", ";
					const types::Variant& colval = si->column(colidx);
					if (colval.defined())
					{
						out << "'" << si->column(colidx).tostring() << "'";
					}
					else
					{
						out << "NULL";
					}
				}
				out << std::endl;
			}
		}
	}
	std::vector<SubroutineSignature>::const_iterator si = signatures.begin(), se = signatures.end();
	if (si != se)
	{
		out << "Subroutine Signatures:" << std::endl;
		for (std::size_t idx=0; si != se; ++si,++idx)
		{
			out << "SUB " << idx << " ( ";
			SubroutineSignature::const_iterator gi = si->begin(), ge = si->end();
			for (std::size_t paramidx=0; gi != ge; ++gi,++paramidx)
			{
				if (paramidx > 0) out << ", ";
				out << *gi;
			}
			out << " )" << std::endl;
		}
	}

	std::vector<ErrorHintList>::const_iterator hi = errorhints.begin(), he = errorhints.end();
	if (hi != he)
	{
		out << "Error Hints:" << std::endl;
		for (std::size_t idx=0; hi != he; ++hi,++idx)
		{
			out << "HINT " << idx << ":" << std::endl;
			ErrorHintList::const_iterator ei = hi->begin(), ee = hi->end();
			for (; ei != ee; ++ei)
			{
				out << "  " << ei->errorclass << ": " << ei->message << std::endl;
			}
			out << std::endl;
		}
	}
}

void ProgramImage::printInstruction( std::ostream& out, const Instruction& instr) const
{
	CondCode cc = condCode( instr);
	const char* ccnam = condCodeName( cc);
	OpCode oc = opCode( instr);
	ArgumentType at = argumentType( oc);
	ArgumentIndex ai = argumentIndex( instr);
	if (ccnam[0])
	{
		out << ccnam << " ";

	}
	out << opCodeName( oc);
	switch (at)
	{
		case At_None:
			break;
		case At_Address:
			out << " @" << ai;
			break;
		case At_Path:
			out << " PATH " << ai;
			break;
		case At_Constant:
			if (ai >= constants.size()) throw std::runtime_error("array bound read");
			out << " CONST \'" << constants.at(ai).tostring() << '\'';
			break;
		case At_ColumnName:
			if (ai == 0 || ai > colnames.size()) throw std::runtime_error("array bound read");
			out << " COLUMN " << colnames[ ai-1];
			break;
		case At_TagName:
			if (ai == 0 || ai > tagnames.size()) throw std::runtime_error("array bound read");
			out << " TAG " << tagnames[ ai-1];
			break;
		case At_ResultName:
			if (ai == 0 || ai > resultnames.size()) throw std::runtime_error("array bound read");
			out << " RESULT " << resultnames[ ai-1];
			break;
		case At_Statement:
			if (ai >= statements.size()) throw std::runtime_error("array bound read");
			out << " STM (" << statements.at( ai) << ")";
			break;
		case At_Hint:
			out << " ERROR HINTS " << ai;
			break;
		case At_SubroutineSignature:
			out << " SIGNATURE " << ai;
			break;
		case At_TupleSet:
			out << " TUPLESET " << ai;
			break;
		case At_SelectedColumnIdx:
			out << " COLIDX " << ai;
			break;
		case At_IteratorColumnIdx:
			out << " COLIDX " << ai;
			break;
	}
}

ProgramImage::ProgramImage( const Program& program)
{
	code = program.code;
	constants = program.constants;
	colnames = program.colnametab.array();
	tagnames = program.tagnametab.array();
	resultnames = program.resultnametab.array();
	statements = program.statements;
	ErrorHintTable::const_iterator hi = program.hinttab.begin(), he = program.hinttab.end();
	for (; hi != he; ++hi)
	{
		errorhints.push_back( ProgramImage::ErrorHintList());

		ErrorHintTable::HintList::const_iterator ei = hi->begin(), ee = hi->end();
		for (; ei != ee; ++ei)
		{
			errorhints.back().push_back( ProgramImage::ErrorHint( ei->errorclass, ei->message));
		}
	}
	signatures = program.signatures;
	tuplesets = program.tuplesets;
}

