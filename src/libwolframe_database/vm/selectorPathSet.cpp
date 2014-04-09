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
//\file vm/selectorPathSet.cpp
//\brief Implementation of a set of input selector pathes
#include "database/vm/selectorPathSet.hpp"
#include "vm/tagTable.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

SelectorPathSet::SelectorPathSet()
	:m_tagtab( new TagTable(false)){}

SelectorPathSet::SelectorPathSet( const SelectorPathSet& o)
	:m_tagtab(new TagTable(*o.m_tagtab))
	,m_pathar(o.m_pathar){}

SelectorPathSet::~SelectorPathSet()
{
	delete m_tagtab;
}

PatchArgumentMapR SelectorPathSet::join( const SelectorPathSet& oth)
{
	std::size_t ofs = m_pathar.size();
	std::vector<SelectorPath>::const_iterator pi = oth.m_pathar.begin(), pe = oth.m_pathar.end();
	for (; pi != pe; ++pi)
	{
		SelectorPath elem( *pi);
		SelectorPath::iterator ei = elem.begin(), ee = elem.end();
		for (; ei != ee; ++ei)
		{
			if (ei->m_tag)
			{
				const char* estr = oth.m_tagtab->getstr( ei->m_tag);
				if (!estr) throw std::logic_error( "patch argument tag name not found");
				ei->m_tag = m_tagtab->get( estr);
			}
		}
		m_pathar.push_back( elem);
	}
	return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
}

