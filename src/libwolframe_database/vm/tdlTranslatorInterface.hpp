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
//\file vm/tdlTranslatorInterface.hpp
//\brief Helper interface for building a virtual machine for database transactions out of TDL
#ifndef _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#define _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#include "database/vm/program.hpp"
#include "database/vm/subroutine.hpp"
#include "types/keymap.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {
namespace vm {

class TdlTranslatorInterface
	:public InstructionSet
{
public:
	explicit TdlTranslatorInterface( const types::keymap<Subroutine>* soubroutinemap_)
		:m_soubroutinemap(soubroutinemap_)
		,m_vm( new Program())
	{}
	TdlTranslatorInterface( const TdlTranslatorInterface& o)
		:m_stateStack(o.m_stateStack)
		,m_unresolvedSubroutineCalls(o.m_unresolvedSubroutineCalls)
		,m_soubroutinemap(o.m_soubroutinemap)
		,m_calledSubroutines(o.m_calledSubroutines)
		,m_vm(o.m_vm)
	{}

	void begin_FOREACH( const std::string& selector);
	void end_FOREACH();

	void begin_DO_statement( const std::string& stm);
	void end_DO_statement();

	void begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues);
	void end_DO_subroutine();

	void push_ARGUMENT_PATH( const std::string& selector);

private:
	struct State
	{
		enum Id {None,OpenForeach,OpenBlock,OpenStatementCall,OpenSubroutineCall};
		Id id;
		InstructionSet::ArgumentIndex value;

		State()
			:id(None),value(0){}
		State( const State& o)
			:id(o.id),value(o.value){}
		State( Id i, InstructionSet::ArgumentIndex v)
			:id(i),value(v){}
	};

	std::vector<State> m_stateStack;
	std::vector<InstructionSet::Address> m_unresolvedSubroutineCalls;
	const types::keymap<Subroutine>* m_soubroutinemap;
	std::vector<Subroutine> m_calledSubroutines;
	ProgramR m_vm;
};

}}}//namespace
#endif

