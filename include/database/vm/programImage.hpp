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
/// \file database/vm/programImage.hpp
/// \brief Interface for program executing database transactions
#ifndef _DATABASE_VM_PROGRAM_IMAGE_HPP_INCLUDED
#define _DATABASE_VM_PROGRAM_IMAGE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "database/vm/programCode.hpp"
#include "database/vm/valueTupleSet.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {
/// \brief Forward declaration
class Program;

/// \class ProgramImage
/// \brief Program addressing only flat data structures and with symbolic information stripped out
class ProgramImage
	:public InstructionSet
{
public:
	typedef std::vector<std::string> SubroutineSignature;
	/// \class ErrorHint
	/// \brief Hint for adding user information to the database error
	struct ErrorHint
	{
		std::string errorclass;
		std::string message;
		ErrorHint(){}
		ErrorHint( const ErrorHint& o)
			:errorclass(o.errorclass),message(o.message){}
		ErrorHint( const std::string& e, const std::string& m)
			:errorclass(e),message(m){}
	};
	typedef std::vector<ErrorHint> ErrorHintList;

public:
	ProgramCode code;				///< program code
	std::vector<types::Variant> constants;		///< constants
	std::vector<std::string> colnames;		///< column names
	std::vector<std::string> tagnames;		///< tag names
	std::vector<std::string> resultnames;		///< result name table
	std::vector<std::string> statements;		///< database statements
	std::vector<ErrorHintList> errorhints;		///< hints for error classes of failing database statements
	std::vector<SubroutineSignature> signatures;	///< subroutine signatures
	std::vector<ValueTupleSetR> tuplesets;		///< value sets from input path expressions

public:
	/// \brief Constructor
	ProgramImage(){}
	/// \brief Copy constructor
	ProgramImage( const ProgramImage& o)
		:code(o.code)
		,constants(o.constants)
		,colnames(o.colnames)
		,tagnames(o.tagnames)
		,resultnames(o.resultnames)
		,statements(o.statements)
		,errorhints(o.errorhints)
		,signatures(o.signatures)
		,tuplesets(o.tuplesets)
	{}

	/// \brief Print one instruction without symbolic information
	void printInstruction( std::ostream& out, const Instruction& instr) const;
	/// \brief Print the whole program without symbolic information
	void print( std::ostream& out) const;
};

}}}//namespace
#endif

