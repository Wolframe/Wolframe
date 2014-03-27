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
#include "transactionfunction/InputStructure.hpp"
#include "transactionfunction/TagTable.hpp"
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace db {

typedef TransactionFunctionInput::Structure::NodeVisitor NodeVisitor;
typedef TransactionFunctionInput::Structure::NodeVisitor::Index NodeIndex;
typedef TransactionFunctionInput::Structure InputStructure;

class SelectorPath
{
public:
	struct Element
	{
		enum Type
		{
			Root,		//< starting '/'
			Next,		//< '/element'
			Find,		//< '//element'
			Up		//< '..'
		};
	
		static const char* typeName( Type i)
		{
			static const char* ar[] ={"Root","Next","Find","Up"};
			return ar[(int)i];
		}
		Type m_type;
		int m_tag;

		Element()
			:m_type(Root),m_tag(0){}
		explicit Element( Type type_, int tag_=0)
			:m_type(type_),m_tag(tag_){}
		Element( const Element& o)
			:m_type(o.m_type),m_tag(o.m_tag){}
	};

	SelectorPath(){}
	SelectorPath( const std::string& selector, TagTable* tagmap);
	SelectorPath( const SelectorPath& o)				:m_path(o.m_path){}

	std::string tostring( const TagTable* tagmap) const;

	void selectNodes( const InputStructure& st, const NodeVisitor& nv, std::vector<NodeIndex>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

private:
	std::vector<Element> m_path;
};



class VirtualMachineTdlTranslatorInterface
	:public VirtualMachine
{
public:
	explicit VirtualMachineTdlTranslatorInterface( bool case_sensitive_)
		:m_tagtab(case_sensitive_)
	{}
	VirtualMachineTdlTranslatorInterface( const VirtualMachineTdlTranslatorInterface& o)
		:VirtualMachine(o)
		,m_tagtab(o.m_tagtab)
		,m_pathar(o.m_pathar)
		,m_blockStack(o.m_blockStack)
		,m_resulttab(o.m_resulttab)
	{}

	void begin_FOREACH( const std::string& selector)
	{
		types::keymap<ArgumentIndex>::const_iterator ri = m_resulttab.find( selector);
		if (ri != m_resulttab.end())
		{
			program
				( Op_OPEN_ITER_KEPT_RESULT, ri->second )// iterate on result named
				( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
			;
		}
		else if (boost::algorithm::iequals( selector, "RESULT"))
		{
			//... selector is referencing the last result
			// Code generated:
			program
				( Op_OPEN_ITER_LAST_RESULT )		// iterate on last result
				( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
			;
		}
		else
		{
			//... selector is referencing a path expression on the input
			vm::InstructionSet::ArgumentIndex idx = m_pathar.size();
			m_pathar.push_back( SelectorPath( selector, &m_tagtab));
	
			// Code generated:
			program
				( Op_OPEN_ITER_PATH, idx)		// iterate on input path
				( Co_IF_COND, Op_GOTO_ABSOLUTE, 0)	// goto end of block if set empty
			;
		}
		m_blockStack.push_back( program.size());
	}

	void end_FOREACH()
	{
		if (m_blockStack.empty()) throw std::runtime_error( "illegal state: end of FOREACH without begin");

		Instruction& forwardJumpInstr = program[ m_blockStack.back()-1];
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

	void begin_CALL( const std::string& name, const std::vector<std::string>& signature)
	{
		vm::SymbolTable::Index subroutineIdx = symboltab.getIndex( name);
		if (subroutineIdx == vm::SymbolTable::UnknownSymbol)
		{
			subroutineIdx = symboltab.define( name);
		}
	}

	void push_ARGUMENT_PATH( const std::string& selector)
	{
		vm::InstructionSet::ArgumentIndex idx = m_pathar.size();
		m_pathar.push_back( SelectorPath( selector, &m_tagtab));

		// Code generated:
		program
			( Op_OPEN_ITER_PATH, idx )			// iterate on input path
		;
	}

private:
	TagTable m_tagtab;
	std::vector<SelectorPath> m_pathar;
	std::vector<Address> m_blockStack;
	types::keymap<ArgumentIndex> m_resulttab;
};

}}//namespace
#endif

