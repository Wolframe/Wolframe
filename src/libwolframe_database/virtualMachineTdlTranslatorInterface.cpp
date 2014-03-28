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
//\file virtualMachineTdlTranslatorInterface.cpp
//\brief Implementation of the helpers for building a virtual machine for database transactions out of TDL
#include "virtualMachineTdlTranslatorInterface.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

void VirtualMachineTdlTranslatorInterface::begin_FOREACH( const std::string& selector)
{
	vm::InstructionSet::ArgumentIndex idx;
	if (0!=(idx=m_vm->resultnametab.getIndex( selector)))
	{
		m_vm->program
			( Op_OPEN_ITER_KEPT_RESULT, idx )	// iterate on result named
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	else if (boost::algorithm::iequals( selector, "RESULT"))
	{
		//... selector is referencing the last result
		// Code generated:
		m_vm->program
			( Op_OPEN_ITER_LAST_RESULT )		// iterate on last result
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	else
	{
		//... selector is referencing a path expression on the input
		idx = m_vm->pathset.add( selector);

		// Code generated:
		m_vm->program
			( Op_OPEN_ITER_PATH, idx)		// iterate on input path
			( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
		;
	}
	m_stateStack.push_back( State( State::OpenForeach, m_vm->program.size()));
}

void VirtualMachineTdlTranslatorInterface::end_FOREACH()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenForeach) throw std::runtime_error( "illegal state: end of FOREACH without begin");

	Instruction& forwardJumpInstr = m_vm->program[ m_stateStack.back().value-1];
	if (forwardJumpInstr != instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, 0))
	{
		throw std::runtime_error( "illegal state: forward patch reference not pointing to instruction expected");
	}
	// Code generated:
	m_vm->program
		( Op_NEXT )
		( Co_IF_COND, Op_GOTO_ABSOLUTE, m_stateStack.back().value)
	;
	// Patch forward jump (if iterator set empty):
	forwardJumpInstr = InstructionSet::instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, m_vm->program.size());
	m_stateStack.pop_back();
}

void VirtualMachineTdlTranslatorInterface::begin_DO_statement( const std::string& stm)
{
	// Code generated:
	m_vm->program
		( Op_STM_START, m_vm->statements.size() )
	;
	m_stateStack.push_back( State( State::OpenStatementCall, m_vm->statements.size()));
	m_vm->statements.push_back( stm);
}

void VirtualMachineTdlTranslatorInterface::end_DO_statement()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenStatementCall) throw std::runtime_error( "illegal state: end of DO statement without begin");
	// Code generated:
	m_vm->program
		( Op_STM_EXEC, m_stateStack.back().value )
	;
	m_stateStack.pop_back();
}

static std::string mangledSubroutineName( const std::string& name, const std::vector<std::string>& templateParamValues)
{
	if (templateParamValues.empty())
	{
		return boost::to_lower_copy( name);
	}
	std::string rt = boost::to_lower_copy( name);
	std::vector<std::string>::const_iterator ti = templateParamValues.begin(), te = templateParamValues.end();
	for (;ti != te; ++ti)
	{
		rt.push_back( '$');
		rt.append( boost::to_upper_copy( *ti));
	}
	return rt;
}

void VirtualMachineTdlTranslatorInterface::begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues)
{
	types::keymap<vm::Subroutine>::const_iterator si = m_soubroutinemap->find( name), se = m_soubroutinemap->end();
	if (si == se)
	{
		if (templateParamValues.empty())
		{
			throw std::runtime_error( std::string("calling undefined subroutine '") + name + "'");
		}
		else
		{
			throw std::runtime_error( std::string("calling undefined subroutine template '") + name + "'");
		}
	}
	else
	{
		vm::InstructionSet::ArgumentIndex subroutineIdx = 0;
		vm::InstructionSet::ArgumentIndex frameIdx = 0;
		
		std::string mangledName = mangledSubroutineName( name, templateParamValues);
		std::vector<vm::Subroutine>::const_iterator ci = m_calledSubroutines.begin(), ce = m_calledSubroutines.end();
		for (; ci != ce && mangledName != ci->name(); ++ci,++subroutineIdx){}
		if (ci == ce)
		{
			subroutineIdx = m_calledSubroutines.size();
			vm::Subroutine sr( si->second);
			if (sr.templateParams().size() != templateParamValues.size())
			{
				throw std::runtime_error( std::string("calling subroutine template '") + name + "' with wrong number of template parameters");
			}
			sr.setName( mangledName);
			if (templateParamValues.size())
			{
				sr.substituteStatementTemplates( templateParamValues);
			}
			m_calledSubroutines.push_back( sr);
		}
		frameIdx = m_vm->signatures.size();
		m_vm->signatures.push_back( m_calledSubroutines.at(subroutineIdx).params());

		// Code generated:
		m_vm->program
			( Op_SUB_FRAME_OPEN, frameIdx )
		;
		m_stateStack.push_back( State( State::OpenSubroutineCall, subroutineIdx));
	}
}

void VirtualMachineTdlTranslatorInterface::end_DO_subroutine()
{
	if (m_stateStack.empty() || m_stateStack.back().id != State::OpenSubroutineCall) throw std::runtime_error( "illegal state: end of DO without begin");
	// Code generated:
	m_vm->program
		( Op_SUB_FRAME_CLOSE )
		( Op_GOTO_ABSOLUTE, m_stateStack.back().value )
	;
	m_stateStack.pop_back();
	m_unresolvedSubroutineCalls.push_back( m_vm->program.size() -1);
}

void VirtualMachineTdlTranslatorInterface::push_ARGUMENT_PATH( const std::string& selector)
{
	vm::InstructionSet::ArgumentIndex idx = m_vm->pathset.add( selector);

	// Code generated:
	m_vm->program
		( Op_OPEN_ITER_PATH, idx )			// iterate on input path
	;
}


