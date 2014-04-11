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
///\file database/vm/programCode.hpp
///\brief Defines a map of symbols to addresses and back
#ifndef _DATABASE_VIRTUAL_MACHINE_PROGRAM_CODE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_PROGRAM_CODE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "database/vm/patchArgumentMap.hpp"
#include <vector>
#include <cstdlib>
#include <limits>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class ProgramCode
{
public:
	typedef InstructionSet::Instruction Instruction;
	typedef InstructionSet::CondCode CondCode;
	typedef InstructionSet::OpCode OpCode;
	typedef InstructionSet::Address Address;

public:
	ProgramCode(){}
	ProgramCode( const ProgramCode& o)
		:m_ar(o.m_ar){}

	void add( const Instruction& instr)
	{
		m_ar.push_back( instr);
		if (m_ar.size() >= InstructionSet::Max_ArgumentIndex)
		{
			throw std::runtime_error( "program code size out of range");
		}
	}
	Instruction& operator[]( const Address& adr)
	{
		if (adr >= m_ar.size()) throw std::runtime_error( "address out of bounds");
		return m_ar[ adr];
	}
	const Instruction& operator[]( const Address& adr) const
	{
		if (adr >= m_ar.size()) throw std::runtime_error( "address out of bounds");
		return m_ar[ adr];
	}

	Address size() const
	{
		return (Address)m_ar.size();
	}

	ProgramCode& operator()( CondCode cond, OpCode opcode, unsigned int arg=0)
	{
		add( InstructionSet::instruction( cond, opcode, arg));
		return *this;
	}
	ProgramCode& operator()( OpCode opcode, unsigned int arg=0)
	{
		CondCode cond = InstructionSet::Co_ALWAYS;
		add( InstructionSet::instruction( cond, opcode, arg));
		return *this;
	}

	void append( const ProgramCode& prg)
	{
		m_ar.insert( m_ar.end(), prg.begin(), prg.end());
	}

	Instruction get( Address& ip) const
	{
		if (ip >= m_ar.size()) return InstructionSet::instruction( InstructionSet::Co_ALWAYS, InstructionSet::Op_EXIT);
		return m_ar.at(ip);
	}

	typedef std::vector<Instruction>::const_iterator const_iterator;
	typedef std::vector<Instruction>::iterator iterator;

	const_iterator begin() const					{return m_ar.begin();}
	const_iterator end() const					{return m_ar.end();}
	iterator begin()						{return m_ar.begin();}
	iterator end()							{return m_ar.end();}
	const_iterator at( const Address& adr) const			{return m_ar.begin() + adr;}
	iterator at( const Address& adr)				{return m_ar.begin() + adr;}

	PatchArgumentMapR join( const ProgramCode& oth)
	{
		std::size_t ofs = m_ar.size();
		m_ar.insert( m_ar.end(), oth.begin(), oth.end());
		return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
	}

	void printRaw( std::ostream& out) const
	{
		InstructionSet::printProgramRaw( out, m_ar);
	}

private:
	std::vector<Instruction> m_ar;
};

}}}//namespace
#endif

