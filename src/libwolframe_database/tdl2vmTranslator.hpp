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
//\file tdl2vmTranslator.hpp
//\brief Local interface for building a virtual machine program for database transactions out of TDL
#ifndef _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#define _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#include "database/vm/program.hpp"
#include "database/vm/instructionSet.hpp"
#include "vm/subroutine.hpp"
#include "types/keymap.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {

class Tdl2vmTranslator
	:public vm::InstructionSet
{
public:
	explicit Tdl2vmTranslator( const types::keymap<vm::Subroutine>* soubroutinemap_);
	Tdl2vmTranslator( const Tdl2vmTranslator& o);

	void begin_FOREACH( const std::string& selector);
	void end_FOREACH();

	void init_resultset();
	void define_resultset_unique();
	void define_resultset_nonempty();

	void begin_INTO_block( const std::string& tag);
	void end_INTO_block();

	void begin_DO_statement( const std::string& stm);
	void statement_HINT( const std::string& errorclass, const std::string& message);
	void end_DO_statement();
	void print_statement_result( const std::vector<std::string>& path);

	//\param[in] selector the selector string that has to be part of the mangled name of the subroutine created and called because references to input pathes may be be different and therefore require the distinguishing of the instances
	void begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues, const std::string& selector);
	void end_DO_subroutine();

	void push_ARGUMENT_LOOPCNT();
	void push_ARGUMENT_PATH( const std::string& selector);
	void push_ARGUMENT_CONST( const types::Variant& value);
	void push_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx);
	void push_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname);

	void print_ARGUMENT_LOOPCNT();
	void print_ARGUMENT_PATH( const std::string& selector);
	void print_ARGUMENT_CONST( const types::Variant& value);
	void print_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx);
	void print_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname);

	void result_KEEP( const std::string& name);

	vm::ProgramR createProgram() const;

private:
	struct State
	{
		enum Id {
			None,
			OpenForeach,
			OpenIntoBlock,
			OpenStatementCall,
			StatementHint,
			OpenSubroutineCall
		};
		Id id;
		vm::InstructionSet::ArgumentIndex value;
		unsigned int cnt;

		State()
			:id(None),value(0),cnt(0){}
		State( const State& o)
			:id(o.id),value(o.value),cnt(o.cnt){}
		State( Id i, vm::InstructionSet::ArgumentIndex v)
			:id(i),value(v),cnt(0){}
	};
	struct CalledSubroutineDef
	{
		Address address;
		vm::Subroutine subroutine;
		std::string mangledName;

		CalledSubroutineDef(){}
		CalledSubroutineDef( const CalledSubroutineDef& o)
			:address(o.address)
			,subroutine(o.subroutine)
			,mangledName(o.mangledName){}
		CalledSubroutineDef( Address a, const vm::Subroutine& s, const std::string& m)
			:address(a)
			,subroutine(s)
			,mangledName(m){}
	};

	std::vector<State> m_stateStack;
	const types::keymap<vm::Subroutine>* m_soubroutinemap;
	std::vector<CalledSubroutineDef> m_calledSubroutines;
	vm::Program m_sub_program;
	vm::Program m_main_program;
};

}}//namespace
#endif

