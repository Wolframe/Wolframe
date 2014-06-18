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
///\file tdl2vmTranslator.hpp
///\brief Local interface for building a virtual machine program for database transactions out of TDL
#ifndef _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#define _DATABASE_TDL_TRANSLATOR_INTERFACE_HPP_INCLUDED
#include "database/vm/instructionSet.hpp"
#include "vm/program.hpp"
#include "vm/subroutine.hpp"
#include "types/keymap.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {

class Tdl2vmTranslator
	:public vm::InstructionSet
{
public:
	Tdl2vmTranslator( const types::keymap<vm::Subroutine>* soubroutinemap_, bool isSubroutine);
	Tdl2vmTranslator( const Tdl2vmTranslator& o);

	void setCurrentSourceReference( const utils::FileLineInfo& posinfo);

	void begin_FOREACH( const std::string& selector);
	void end_FOREACH();

	void init_resultset();
	void define_resultset_unique();
	void define_resultset_nonempty();

	void begin_INTO_block( const std::vector<std::string>& path);
	void begin_INTO_block( const std::string& name);
	void end_INTO_block();

	void begin_loop_INTO_block( const std::vector<std::string>& path);
	void end_loop_INTO_block();

	void begin_loop_element();
	void end_loop_element();

	void begin_DO_statement( const std::string& stm);
	void statement_HINT( const std::string& errorclass, const std::string& message);
	void end_DO_statement();

	void output_statement_result( bool isLoop);

	///\param[in] name name of the subroutine
	///\param[in] templateParamValues template parameter of the subroutine
	///\param[in] selector the selector string that has to be part of the mangled name of the subroutine created and called because references to input pathes may be be different and therefore require the distinguishing of the instances
	void begin_DO_subroutine( const std::string& name, const std::vector<std::string>& templateParamValues, const std::string& selector);
	void end_DO_subroutine();

	void push_ARGUMENT_LOOPCNT();
	void push_ARGUMENT_PATH( const std::string& selector);
	void push_ARGUMENT_CONST( const types::Variant& value);
	void push_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx);
	void push_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname);
	void push_ARGUMENT_ITER( unsigned int colidx);
	void push_ARGUMENT_ITER( const std::string& colname);

	void output_ARGUMENT_LOOPCNT();
	void output_ARGUMENT_PATH( const std::string& selector);
	void output_ARGUMENT_CONST( const types::Variant& value);
	void output_ARGUMENT_TUPLESET( const std::string& setname, unsigned int colidx);
	void output_ARGUMENT_TUPLESET( const std::string& setname, const std::string& colname);
	void output_ARGUMENT_ITER( unsigned int colidx);
	void output_ARGUMENT_ITER( const std::string& colname);

	void result_KEEP( const std::string& name);

	void add_auditcall( const vm::Program& auditcallprg);
	
	vm::ProgramR createProgram( bool withReturn=true) const;

private:
	struct State
	{
		enum Id {
			None,
			OpenForeach,
			OpenIntoBlock,
			OpenIntoBlockArray,
			OpenStatementCall,
			StatementHint,
			OpenSubroutineCall,
			OpenLoopElement
		};
		Id id;
		vm::InstructionSet::ArgumentIndex value;	//< state variable (interpretation is depending on id)
		unsigned int paramcnt;				//< counter of parameters pushed for validity check

		State()
			:id(None),value(0),paramcnt(0){}
		State( const State& o)
			:id(o.id),value(o.value),paramcnt(o.paramcnt){}
		State( Id i, vm::InstructionSet::ArgumentIndex v)
			:id(i),value(v),paramcnt(0){}
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

	std::vector<State> m_stateStack;			///< state stack to verify output and store context information
	const types::keymap<vm::Subroutine>* m_soubroutinemap;	///< map of subroutines that can be called in this context
	std::vector<CalledSubroutineDef> m_calledSubroutines;	///< list of subroutines called to eliminate duplicates (identical mangled name references)
	vm::Program m_sub_program;				///< code segment where all subroutines called are added
	vm::Program m_main_program;				///< code segment of the main program
	bool m_isSubroutine;					///< bool if this program is a subroutine
	unsigned int m_nofCommands;				///< counter of commands executed
};

}}//namespace
#endif

