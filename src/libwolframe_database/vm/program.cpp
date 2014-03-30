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
//\file vm/program.cpp
//\brief Implementation of the virtual machine program
#include "database/vm/program.hpp"
#include "database/vm/patchArgumentMap.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::db::vm;

template <class Element>
static PatchArgumentMapR joinVectors( std::vector<Element>& dst, const std::vector<Element>& ar)
{
	std::size_t ofs = dst.size();
	dst.insert( dst.end(), ar.begin(), ar.end());
	return PatchArgumentMapR( new PatchArgumentMap_Offset( ofs));
}

static void patchCode( ProgramCode& code, InstructionSet::ArgumentType argtype, std::size_t startidx, const PatchArgumentMap& pamap)
{
	if (startidx >= code.size()) throw std::logic_error("array bound access");
	ProgramCode::iterator ci = code.begin() + startidx, ce = code.end();
	for (; ci != ce; ++ci)
	{
		InstructionSet::OpCode oc = InstructionSet::opCode( *ci);
		if (argtype == InstructionSet::argumentType( oc))
		{
			InstructionSet::CondCode cc = InstructionSet::condCode( *ci);
			InstructionSet::ArgumentIndex ai = InstructionSet::argumentIndex( *ci);
			*ci = InstructionSet::instruction( cc, oc, pamap[ ai]);
		}
	}
}

void Program::add( const Program& oth, bool doPatchGOTOs)
{
	std::size_t startidx = code.size();
	PatchArgumentMapR pmap;
	if (doPatchGOTOs)
	{
		pmap = code.join( oth.code);
		patchCode( code, At_Address, startidx, *pmap);
	}
	else
	{
		code.append( oth.code);
	}

	pmap = joinVectors( constants, oth.constants);
	patchCode( code, At_Constant, startidx, *pmap);

	pmap = tagnametab.join( oth.tagnametab);
	patchCode( code, At_TagName, startidx, *pmap);

	pmap = resultnametab.join( oth.resultnametab);
	patchCode( code, At_ResultName, startidx, *pmap);

	pmap = colnametab.join( oth.colnametab);
	patchCode( code, At_ColumnName, startidx, *pmap);

	pmap = joinVectors( statements, oth.statements);
	patchCode( code, At_Statement, startidx, *pmap);

	pmap = joinVectors( signatures, oth.signatures);
	patchCode( code, At_SubroutineSignature, startidx, *pmap);

	pmap = pathset.join( oth.pathset);
	patchCode( code, At_Path, startidx, *pmap);
}

