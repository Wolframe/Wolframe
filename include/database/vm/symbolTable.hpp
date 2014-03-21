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
//\file database/vm/symbolTable.hpp
//\brief Defines a map of symbols to addresses and back
#ifndef _DATABASE_VIRTUAL_MACHINE_SYMBOL_TABLE_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SYMBOL_TABLE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "types/keymap.hpp"
#include <map>
#include <vector>
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace db {
namespace vm {

class SymbolTable
{
public:
	typedef InstructionSet::Address Address;
	typedef InstructionSet::ArgumentIndex Index;
	enum {
		Max_Adress	=	(1<<20)-1,
		Null		=	0
	};
	enum {
		UnresolvedSymbol=	(1<<20),
		UnknownSymbol	=	0xFFFFffff
	};

public:
	SymbolTable(){}
	SymbolTable( const SymbolTable& o)
		:m_symbolmap(o.m_symbolmap)
		,m_symbolar(o.m_symbolar)
		,m_addressar(o.m_addressar){}

	const std::string& getSymbol( const Index& idx) const
	{
		if (idx > m_symbolar.size()) throw std::runtime_error("symbol reference out of range");
		if (idx == Null) throw std::runtime_error("symbol reference is NULL");
		return m_symbolar.at(idx-1);
	}

	Address getAddress( const Index& idx) const
	{
		if (idx > m_addressar.size()) throw std::runtime_error("symbol reference out of range");
		if (idx == Null) throw std::runtime_error("symbol reference is NULL");
		return m_addressar.at(idx-1);
	}

	Address getAddress( const std::string& sym) const
	{
		SymbolMap::const_iterator si = m_symbolmap.find( sym);
		if (si == m_symbolmap.end()) return UnknownSymbol;
		if (si->second == Null) return UnresolvedSymbol; 
		return m_addressar.at( si->second);
	}

	Index getIndex( const std::string& sym) const
	{
		SymbolMap::const_iterator si = m_symbolmap.find( sym);
		if (si == m_symbolmap.end()) return UnknownSymbol;
		return si->second;
	}

	Index define( const std::string& sym, const Address& adr=Null)
	{
		m_symbolmap.insert( sym, m_symbolar.size());
		m_symbolar.push_back( sym);
		m_addressar.push_back( adr);
		return m_symbolar.size();
	}

	void resolve( const std::string& sym, const Address& adr)
	{
		SymbolMap::const_iterator si = m_symbolmap.find( sym);
		if (si == m_symbolmap.end()) throw std::runtime_error("internal: could not resolve symbol is translation of database transaction");
		m_addressar[ si->second] = adr;
	}

	void resolve( const Index& idx, const Address& adr)
	{
		if (idx > m_symbolar.size()) throw std::runtime_error("symbol reference out of range");
		if (idx == Null) throw std::runtime_error("symbol reference is NULL");
		m_addressar[ idx-1] = adr;
	}

private:
	typedef types::keymap<Index> SymbolMap;
	SymbolMap m_symbolmap;
	std::vector<std::string> m_symbolar;
	std::vector<Address> m_addressar;
};

typedef boost::shared_ptr<SymbolTable> SymbolTableR;

}}}//namespace
#endif

