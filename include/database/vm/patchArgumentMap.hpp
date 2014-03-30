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
//\file database/vm/patchArgumentMap.hpp
//\brief Interface for patching program code needed for joining programs
#ifndef _DATABASE_VM_PATCH_ARGUMENT_MAP_HPP_INCLUDED
#define _DATABASE_VM_PATCH_ARGUMENT_MAP_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include <map>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class PatchArgumentMap
{
public:
	typedef vm::InstructionSet::ArgumentIndex ArgumentIndex;
public:
	virtual ~PatchArgumentMap(){}
	virtual ArgumentIndex operator[]( ArgumentIndex i) const=0;
};

typedef boost::shared_ptr<PatchArgumentMap> PatchArgumentMapR;


class PatchArgumentMap_Offset
	:public PatchArgumentMap
{
public:
	PatchArgumentMap_Offset( ArgumentIndex ofs_)
		:m_ofs(ofs_){}
	virtual ~PatchArgumentMap_Offset(){}
	virtual ArgumentIndex operator[]( ArgumentIndex i) const
	{
		return m_ofs + i;
	}

private:
	ArgumentIndex m_ofs;
};

class PatchArgumentMap_Table
	:public PatchArgumentMap
{
public:
	PatchArgumentMap_Table( const std::map<ArgumentIndex,ArgumentIndex>& map_)
		:m_map(map_){}
	virtual ~PatchArgumentMap_Table(){}
	virtual ArgumentIndex operator[]( ArgumentIndex i) const
	{
		std::map<ArgumentIndex,ArgumentIndex>::const_iterator mi = m_map.find( i);
		if (mi == m_map.end()) throw std::logic_error( "incomplete patch argument map");
		return mi->second;
	}

private:
	std::map<ArgumentIndex,ArgumentIndex> m_map;
};


}}}//namespace
#endif

