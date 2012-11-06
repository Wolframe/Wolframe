/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\brief Definition of transaction input
///\file database/transactionInput.hpp
#ifndef _DATABASE_TRANSACTION_INPUT_HPP_INCLUDED
#define _DATABASE_TRANSACTION_INPUT_HPP_INCLUDED
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

namespace _Wolframe {
namespace db {

class TransactionInput
{
public:
	TransactionInput()
		:m_strings("\0\0",2){}
	TransactionInput( const TransactionInput& o)
		:m_cmd(o.m_cmd)
		,m_strings(o.m_strings){}

	class Element
	{
	public:
		enum Type
		{
			String,
			ResultColumn
		};

		Element(){}
		Element( Type type_, std::size_t idx_)
			:m_type( type_)
			,m_idx(idx_){}
		Element( const Element& o)
			:m_type(o.m_type)
			,m_idx(o.m_idx){}

		Type type() const					{return m_type;}
		std::size_t idx() const					{return m_idx;}

	private:
		Type m_type;
		std::size_t m_idx;
	};

	class Command
	{
	public:
		Command()
			:m_functionidx(0){}
		Command( std::size_t functionidx_, const std::string& name_)
			:m_functionidx(functionidx_)
			,m_name(name_){}
		Command( const Command& o)
			:m_functionidx(o.m_functionidx)
			,m_name(o.m_name)
			,m_arg(o.m_arg){}

		void bind( Element::Type type, std::size_t idx)
		{
			m_arg.push_back( Element( type, idx));
		}

		std::size_t functionidx() const				{return m_functionidx;}
		const std::string& name() const				{return m_name;}
		const std::vector<Element>& arg() const			{return m_arg;}

	private:
		std::size_t m_functionidx;
		std::string m_name;
		std::vector<Element> m_arg;
	};

	class arg_iterator
	{
	public:
		arg_iterator()
			:m_ref(0){}
		arg_iterator( const arg_iterator& o)
			:m_ref(o.m_ref)
			,m_itr(o.m_itr)
			,m_end(o.m_end)
		{
			initContent();
		}
		arg_iterator( const TransactionInput* ref_, const std::vector<Element>::const_iterator& itr_, const std::vector<Element>::const_iterator& end_)
			:m_ref(ref_)
			,m_itr(itr_)
			,m_end(end_)
		{
			initContent();
		}

		bool compare( const arg_iterator& o) const		{return m_itr==o.m_itr;}
		bool operator==( const arg_iterator& o) const		{return compare(o);}
		bool operator!=( const arg_iterator& o) const		{return !compare(o);}

		arg_iterator& operator++()				{++m_itr; initContent(); return *this;}
		arg_iterator operator++(int)				{arg_iterator rt(*this); ++m_itr; initContent(); return rt;}

	public:
		class Content
		{
		public:
			Content()
				:m_type(Element::String)
				,m_value(0)
				,m_ref(0){}

			Content( const Content& o)
				:m_type(o.m_type)
				,m_value(o.m_value)
				,m_ref(o.m_ref){}

			void init( const char* value_)
			{
				m_type = Element::String;
				m_value = value_;
				m_ref = 0;
			}

			void init( std::size_t ref_)
			{
				m_type = Element::ResultColumn;
				m_value = 0;
				m_ref = ref_;
			}

			Element::Type type() const	{return m_type;}
			const char* value() const	{return m_value;}
			std::size_t ref() const		{return m_ref;}

		private:
			Element::Type m_type;
			const char* m_value;
			std::size_t m_ref;
		};

		const Content& operator*() const			{return m_content;}
		const Content* operator->() const			{return &m_content;}

	private:
		void initContent()
		{
			if (m_itr == m_end)
			{
				m_content.init( (const char*)0);
			}
			else
			{
				switch (m_itr->type())
				{
					case Element::String:
						m_content.init( m_ref->value( m_itr->idx()));
						break;
					case Element::ResultColumn:
						m_content.init( m_itr->idx());
						break;
				}
			}
		}
		Content m_content;
		const TransactionInput* m_ref;
		std::vector<Element>::const_iterator m_itr;
		std::vector<Element>::const_iterator m_end;
	};

	class cmd_iterator
	{
	public:
		cmd_iterator(){}
		cmd_iterator( const cmd_iterator& o)
			:m_content(o.m_content){}
		cmd_iterator( const TransactionInput* ref_, const std::vector<Command>::const_iterator& itr_)
			:m_content(ref_, itr_){}

		bool compare( const cmd_iterator& o) const		{return m_content.m_itr==o.m_content.m_itr;}
		bool operator==( const cmd_iterator& o) const		{return compare(o);}
		bool operator!=( const cmd_iterator& o) const		{return !compare(o);}

		cmd_iterator& operator++()				{++m_content.m_itr; return *this;}
		cmd_iterator operator++(int)				{cmd_iterator rt(*this); ++m_content.m_itr; return rt;}

		struct Content
		{
			const TransactionInput* m_ref;
			std::vector<Command>::const_iterator m_itr;

			const std::string& name() const			{return m_itr->name();}
			std::size_t functionidx() const			{return m_itr->functionidx();}

			arg_iterator begin() const			{return arg_iterator(m_ref, m_itr->arg().begin(), m_itr->arg().end());}
			arg_iterator end() const			{return arg_iterator(m_ref, m_itr->arg().end(), m_itr->arg().end());}

			Content()
				:m_ref(0){}
			Content( const Content& o)
				:m_ref(o.m_ref)
				,m_itr(o.m_itr){}
			Content( const TransactionInput* ref_, const std::vector<Command>::const_iterator& itr_)
				:m_ref(ref_)
				,m_itr(itr_){}
		};

		const Content& operator*() const			{return m_content;}
		const Content* operator->() const			{return &m_content;}

	private:
		Content m_content;
	};

	cmd_iterator begin() const					{return cmd_iterator( this, m_cmd.begin());}
	cmd_iterator end() const					{return cmd_iterator( this, m_cmd.end());}

	const char* value( std::size_t idx) const			{return (idx)?(m_strings.c_str()+idx):0;}

	std::size_t getValueIdx( const std::string& v)
	{
		if (v.empty()) return 1;
		std::size_t rt = m_strings.size();
		m_strings.append( v);
		m_strings.push_back('\0');
		return rt;
	}

	std::size_t getValueIdx( const char* v)
	{
		if (!v) return 0;
		if (!*v) return 1;
		std::size_t rt = m_strings.size();
		m_strings.append( v);
		m_strings.push_back('\0');
		return rt;
	}

public:
	///\brief Start new command statement
	///\param[in] stmname name of prepared statement
	void startCommand( std::size_t functionidx, const std::string& stmname)
	{
		m_cmd.push_back( Command( functionidx, stmname));
	}

	///\brief Bind parameter value on current command statement
	void bindCommandArgAsValue( const std::string value_)
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Element::String, getValueIdx( value_));
	}

	///\brief Bind parameter value on current command statement
	void bindCommandArgAsNull()
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Element::String, 0);
	}

	///\brief Bind parameter value on current command statement
	void bindCommandArgAsResultReference( std::size_t resultref)
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Element::ResultColumn, resultref);
	}

	bool hasNonemptyResult( std::size_t functionidx) const
	{
		if (functionidx >= m_nonemptyResult.size()) return false;
		return m_nonemptyResult.at( functionidx);
	}

	void setNonemptyResult( std::size_t functionidx)
	{
		std::size_t nn = (functionidx >= m_nonemptyResult.size())?(functionidx-m_nonemptyResult.size()+1):0;
		for (; nn>0; --nn) m_nonemptyResult.push_back(false);
		m_nonemptyResult[ functionidx] = true;
	}

private:
	std::vector<Command> m_cmd;
	std::string m_strings;
	std::vector<bool> m_nonemptyResult;
};


}}
#endif


