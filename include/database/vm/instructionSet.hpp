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
//\file database/vm/instructionSet.hpp
//\brief Defines the instruction set of the virtual machine defining database transactions
#ifndef _DATABASE_VIRTUAL_MACHINE_INSTRUCTION_SET_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_INSTRUCTION_SET_HPP_INCLUDED
#include <stdexcept>
#include <boost/cstdint.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

struct InstructionSet
{
public:
	enum OpCode
	{
		// Control Flow Instructions:
		Op_EXIT,			//< exit (abort)
		Op_RETURN,			//< return from subroutine or termination of transaction
		Op_GOTO_FORWARD,		//< goto relative address jump forward
		Op_GOTO_BACKWARD,		//< goto relative address jump back
		Op_GOTO_ABSOLUTE,		//< goto absolute address
		Op_GOTO_SYMBOLIC,		//< goto symbolic absolute address (index in names)

		// Print Instructions:
		Op_PRINT_CONST,			//< print a value to output: constant
		Op_PRINT_SEL_IDX,		//< print a value to output: element in selected set (unique, one element set) adressed by column index
		Op_PRINT_SEL_NAM,		//< print a value to output: element in selected set (unique, one element set) adressed by column name
		Op_PRINT_ITR_IDX,		//< print a value to output: element in tuple set iterated adressed by column index
		Op_PRINT_ITR_NAM,		//< print a value to output: element in tuple set iterated adressed by column name
		Op_PRINT_OPEN,			//< print an open tag to output (index in names)
		Op_PRINT_CLOSE,			//< print a close tag to output (no argument)

		// Assignment Instructions:
		Op_KEEP_RESULT,			//< keep result
		Op_SELECT_PARAMETER,		//< set value set selected as the parameter list of the subroutine executed
		Op_SELECT_LAST_RESULT,		//< set value set selected as the last result
		Op_SELECT_KEPT_RESULT,		//< set value set selected as a result named with KEEP

		// Iterator Instructions:
		Op_OPEN_ITER_LAST_RESULT,	//< open a value iterator (set cond flag on !empty): last result 
		Op_OPEN_ITER_KEPT_RESULT,	//< open a value iterator (set cond flag on !empty): index is referecing a kept result index
		Op_OPEN_ITER_PATH,		//< open a value iterator (set cond flag on !empty): index is referecing a path
		Op_OPEN_ITER_TUPLESET,		//< open a value iterator (set cond flag on !empty): index is referecing a tuple set
		Op_NEXT,			//< fetch the next value in iteration

		// Subroutine Call Instructions:
		Op_SUB_FRAME_OPEN,		//< start prepare a subroutine call
		Op_SUB_ARG_CONST,		//< push argument for subroutine call: constant value
		Op_SUB_ARG_SEL_IDX,		//< push argument for subroutine call: element in selected set (unique, one element set) adressed by column index
		Op_SUB_ARG_SEL_NAM,		//< push argument for subroutine call: element in selected set (unique, one element set) adressed by column name
		Op_SUB_ARG_ITR_IDX,		//< push argument for subroutine call: element in tuple set iterated adressed by column index
		Op_SUB_ARG_ITR_NAM,		//< push argument for subroutine call: element in tuple set iterated adressed by column name
		Op_SUB_FRAME_CLOSE,		//< push state with parameters on stack for subroutine call. For the jump to the subroutine GOTO is used.

		// Database Instructions:
		Op_STM_START,			//< open a statement to execute
		Op_STM_BIND_CONST,		//< bind a statement parameter: constant value
		Op_STM_BIND_SEL_IDX,		//< bind a statement parameter
		Op_STM_BIND_SEL_NAM,		//< bind a statement parameter
		Op_STM_BIND_ITR_IDX,		//< bind a statement parameter
		Op_STM_BIND_ITR_NAM,		//< bind a statement parameter
		Op_STM_EXEC,			//< execute open statement

		// Collect Results and Constraints:
		Op_RESULT_INIT,
		Op_RESULT_INIT_UNIQUE,
		Op_RESULT_INIT_NONEMPTY,
		Op_RESULT_INIT_NONEMPTY_UNIQUE,

		// Others:
		Op_NOP				//< no operation
	};
	enum CondCode
	{
		Co_ALWAYS,			//< execute always
		Co_IF_COND,			//< execute, if cond flag set to true
		Co_NOT_IF_COND			//< execute, if cond flag set to false
	};
	typedef boost::uint32_t ArgumentIndex;
	typedef boost::uint32_t Address;
	typedef boost::uint32_t Instruction;

	static Instruction instruction( CondCode cond, OpCode opcode, unsigned int arg=0)
	{
		Instruction rt = 0;
		if ((unsigned int)cond > Max_CondCode) throw std::runtime_error( "VM instruction condition code out of range");
		rt |= (Instruction)cond << Shift_CondCode;
		if ((unsigned int)opcode > Max_OpCode) throw std::runtime_error( "VM instruction opcode out of range");
		rt |= (Instruction)opcode << Shift_OpCode;
		if (arg > Max_ArgumentIndex) throw std::runtime_error( "VM instruction parameter reference out of range");
		rt |= (arg << Shift_ArgumentIndex);
		return rt;
	}
	static Instruction instruction( OpCode opcode, unsigned int arg=0)
	{
		return instruction( Co_ALWAYS, opcode, arg);
	}

	static CondCode condCode( const Instruction& instr)		{return static_cast<CondCode>((unsigned int)(instr&Mask_CondCode) >> Shift_CondCode);}
	static OpCode opCode( const Instruction& instr)			{return static_cast<OpCode>((unsigned int)(instr&Mask_OpCode) >> Shift_OpCode);}
	static ArgumentIndex argumentIndex( const Instruction& instr)	{return static_cast<ArgumentIndex>((unsigned int)(instr&Mask_ArgumentIndex) >> Shift_ArgumentIndex);}

private:
	enum {
		BitCnt_OpCode=6,	Shift_OpCode=26,
		BitCnt_CondCode=2,	Shift_CondCode=24,
		BitCnt_ArgumentIndex=20,Shift_ArgumentIndex=0
	};
public:
	enum {
		Max_OpCode=((1<<(BitCnt_OpCode+1))-1),
		Max_CondCode=((1<<(BitCnt_CondCode+1))-1),
		Max_ArgumentIndex=((1<<(BitCnt_ArgumentIndex+1))-1)
	};
private:
	enum {
		Mask_OpCode=(Max_OpCode << Shift_OpCode),
		Mask_CondCode=(Max_CondCode << Shift_CondCode),
		Mask_ArgumentIndex=(Max_ArgumentIndex << Shift_ArgumentIndex)
	};
};

}}}//namespace
#endif

