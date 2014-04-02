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
//\file database/vm/selectorPathSet.hpp
//\brief Defines a set selector pathes
#ifndef _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_SET_HPP_INCLUDED
#define _DATABASE_VIRTUAL_MACHINE_SELECTOR_PATH_SET_HPP_INCLUDED
#include "database/vm/selectorPath.hpp"
#include "database/vm/instructionSet.hpp"
#include "database/vm/patchArgumentMap.hpp"
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/TagTable.hpp"

namespace _Wolframe {
namespace db {
namespace vm {

class SelectorPathSet
{
public:
	typedef InstructionSet::ArgumentIndex Index;

public:
	explicit SelectorPathSet()
		:m_tagtab(false){}
	SelectorPathSet( const SelectorPathSet& o)
		:m_tagtab(o.m_tagtab)
		,m_pathar(o.m_pathar){}

	const SelectorPath& getPath( const Index& idx) const
	{
		if (idx == 0) throw std::runtime_error( "accessing null selector path");
		if (idx >= m_pathar.size()) throw std::runtime_error( "selector path index out of bounds");
		return m_pathar.at( idx-1);
	}

	Index add( const std::string& selector)
	{
		m_pathar.push_back( SelectorPath( selector, &m_tagtab));
		return m_pathar.size();
	}

	PatchArgumentMapR join( const SelectorPathSet& oth)
	{
		std::size_t ofs = m_pathar.size();
		std::vector<SelectorPath>::const_iterator pi = oth.m_pathar.begin(), pe = oth.m_pathar.end();
		for (; pi != pe; ++pi)
		{
			SelectorPath elem( *pi);
			SelectorPath::iterator ei = elem.begin(), ee = elem.end();
			for (; ei != ee; ++ei)
			{
				const char* estr = oth.m_tagtab.getstr( ei->m_tag);
				if (!estr) throw std::logic_error( "patch argument tag name not found");
				ei->m_tag = m_tagtab.get( estr);
			}
			m_pathar.push_back( elem);
		}
		return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
	}

	const TagTable& tagtab() const
	{
		return m_tagtab;
	}
	TagTable& tagtab()
	{
		return m_tagtab;
	}

private:
	TagTable m_tagtab;
	std::vector<SelectorPath> m_pathar;	
};

}}}//namespace
#endif

