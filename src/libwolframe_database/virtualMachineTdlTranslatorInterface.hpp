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
//\file virtualMachineTdlTranslatorInterface.hpp
//\brief Helper interface for building a virtual machine for database transactions out of TDL
#ifndef _DATABASE_VIRTUAL_MACHINE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#include "database/virtualMachine.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {

class VirtualMachineTdlTranslatorInterface
{
public:
	explicit VirtualMachineTdlTranslatorInterface( const types::keymap<Subroutine>* soubroutinemap_)
		:m_soubroutinemap(soubroutinemap_)
		,m_vm( new VirtualMachine())
	{}
	VirtualMachineTdlTranslatorInterface( const VirtualMachineTdlTranslatorInterface& o)
		:m_blockStack(o.m_blockStack)
		,m_soubroutinemap(o.m_soubroutinemap)
		,m_vm(o.m_vm)
	{}

	void begin_FOREACH( const std::string& selector)
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
			program
				( Op_OPEN_ITER_PATH, idx)		// iterate on input path
				( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
			;
		}
		m_blockStack.push_back( m_vm->program.size());
	}

	void end_FOREACH()
	{
		if (m_blockStack.empty()) throw std::runtime_error( "illegal state: end of FOREACH without begin");

		Instruction& forwardJumpInstr = m_vm->program[ m_blockStack.back()-1];
		if (forwardJumpInstr != instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, 0))
		{
			throw std::runtime_error( "illegal state: forward patch reference not pointing to instruction expected");
		}
		// Code generated:
		program
			( Op_NEXT )
			( Co_IF_COND, Op_GOTO_ABSOLUTE, m_blockStack.back())
		;
		// Patch forward jump (if iterator set empty):
		forwardJumpInstr = InstructionSet::instruction( Co_IF_COND, Op_GOTO_ABSOLUTE, program.size());
	}

	void begin_DO( const std::string& name)
	{
		types::keymap<Subroutine>::const_iterator si = m_soubroutinemap->find( name), se = m_soubroutinemap->end();
		if (si == se)
		{
			!!!! STATEMENT
		}
		else
		{
			!!!! SUBROUTINE CALL
		}
	}

	void push_ARGUMENT_PATH( const std::string& selector)
	{
		vm::InstructionSet::ArgumentIndex idx = m_vm->pathset.add( selector);

		// Code generated:
		program
			( Op_OPEN_ITER_PATH, idx )			// iterate on input path
		;
	}

private:
	std::vector<Address> m_blockStack;
	const types::keymap<Subroutine>* m_soubroutinemap;
	VirtualMachineR m_vm;
};

}}//namespace
#endif

