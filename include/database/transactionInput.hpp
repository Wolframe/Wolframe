/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class TransactionInput
///\brief Input of a transaction
class TransactionInput
{
public:
	///\brief Constructor
	TransactionInput(){}
	///\brief Copy constructor
	TransactionInput( const TransactionInput& o)
		:m_cmd(o.m_cmd){}

	///\class Command
	///\brief Structure representing one command in a transaction
	class Command
	{
	public:
		///\class Argument
		///\brief Structure representing one argument of a command in a transaction
		class Argument
		{
		public:
			enum Type
			{
				Value,
				ResultColumn
			};

			///\brief Default constructor
			Argument()
				:m_type(Value),m_scope_functionidx(-1){}
			///\brief Constructor
			Argument( Type type_, const types::Variant& value_, int scope_functionidx_=-1)
				:m_type( type_),m_value(value_),m_scope_functionidx(scope_functionidx_){}
			///\brief Copy constructor
			Argument( const Argument& o)
				:m_type(o.m_type),m_value(o.m_value),m_scope_functionidx(o.m_scope_functionidx){}

			Type type() const			{return m_type;}
			int scope_functionidx() const		{return m_scope_functionidx;}
			const types::Variant& value() const	{return m_value;}

		private:
			Type m_type;				//< type of the argument value
			types::Variant m_value;			//< value of the argument. interpretation depends on type
			int m_scope_functionidx;		//< functionidx describing the scope in case of a result reference
		};
	public:
		///\brief Default constructor
		Command()
			:m_flags(0)
			,m_functionidx(0)
			,m_level(0){}
		///\brief Constructor
		Command( std::size_t functionidx_, std::size_t level_, const std::string& name_)
			:m_flags(0)
			,m_functionidx(functionidx_)
			,m_level(level_)
			,m_name(name_){}
		///\brief Copy constructor
		Command( const Command& o)
			:m_flags(o.m_flags)
			,m_functionidx(o.m_functionidx)
			,m_level(o.m_level)
			,m_name(o.m_name)
			,m_arg(o.m_arg){}

		///\brief Bind value the the next argument of a command
		void bind( Argument::Type type, const types::Variant& value, int scope_functionidx=-1)
		{
			m_arg.push_back( Argument( type, value, scope_functionidx));
		}

		///\brief Get the scope level of this command
		std::size_t level() const			{return m_level;}
		///\brief Get the id of the associated function description
		std::size_t functionidx() const			{return m_functionidx;}
		///\brief Get the name of the command
		const std::string& name() const			{return m_name;}
		///\brief Get the list of arguments of the command
		const std::vector<Argument>& arg() const	{return m_arg;}

		///\brief Test if he result of this command is defined as non empty
		bool nonemptyResult() const			{return flags( NonEmptyResult);}
		///\brief Define the result to be non empty
		void setNonemptyResult( bool v=true)		{setFlags( NonEmptyResult, v);}

		///\brief Test if he result of this command is defined as unique
		bool uniqueResult() const			{return flags( UniqueResult);}
		///\brief Define the result to be unique
		void setUniqueResult( bool v=true)		{setFlags( UniqueResult, v);}

	public:
		typedef std::vector<Argument>::const_iterator arg_const_iterator;
		typedef std::vector<Argument>::iterator arg_iterator;

		arg_const_iterator begin() const			{return m_arg.begin();}
		arg_iterator begin()					{return m_arg.begin();}

		arg_const_iterator end() const				{return m_arg.end();}
		arg_iterator end()					{return m_arg.end();}

	private:
		enum Flags
		{
			None=0x0,
			NonEmptyResult=0x1,
			UniqueResult=0x2
		};

		bool flags( Flags flags_) const			{return ((unsigned char)m_flags & (unsigned char)flags_) == (unsigned char)flags_;}
		void setFlags( Flags flags_, bool v=true)	{if (v) m_flags = (Flags)((unsigned char)m_flags | (unsigned char)flags_); else m_flags = (Flags)((unsigned char)m_flags - ((unsigned char)m_flags & (unsigned char)flags_));}

	private:
		unsigned char m_flags;				//< Flags defining some constraints on the command result (bitfield for enum Flags)
		std::size_t m_functionidx;			//< Identifier used to associate a function description with a result for output with markup
		std::size_t m_level;				//< Identifier used to implement the scope of values that can be referenced by this command
		std::string m_name;				//< Name of the command (internal name for embedded database instructions)
		std::vector<Argument> m_arg;			//< List of arguments passed to the command
	};

public:
	typedef std::vector<Command>::const_iterator cmd_const_iterator;
	typedef std::vector<Command>::iterator cmd_iterator;

	cmd_const_iterator begin() const			{return m_cmd.begin();}
	cmd_iterator begin()					{return m_cmd.begin();}

	cmd_const_iterator end() const				{return m_cmd.end();}
	cmd_iterator end()					{return m_cmd.end();}

public:
	///\brief Start new command statement
	///\param[in] functionidx index of the function in the list of calls in the transaction definition (starting with 0)
	///\param[in] level in the hierarchy of operations to determine begin and end of an operation (starting with 1, incremented by one for the scope a sub operation)
	///\param[in] stmname name of prepared statement
	void startCommand( std::size_t functionidx, std::size_t level, const std::string& stmname)
	{
		m_cmd.push_back( Command( functionidx, level, stmname));
	}

	///\brief Bind parameter value on current command statement
	void bindCommandArgAsValue( const types::Variant& value)
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Command::Argument::Value, value);
	}

	///\brief Bind parameter value on current command statement
	void bindCommandArgAsNull()
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Command::Argument::Value, types::Variant());
	}

	///\brief Bind parameter value on current or previous command result
	void bindCommandArgAsResultReference( std::size_t resultref, int scope_functionidx=-1)
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Command::Argument::ResultColumn, types::Variant((unsigned int)resultref), scope_functionidx);
	}

	///\brief Bind parameter value on current or previous command result
	void bindCommandArgAsResultReference( const std::string& resultsymbol, int scope_functionidx=-1)
	{
		if (m_cmd.empty()) throw std::logic_error( "bind called with no command defined");
		m_cmd.back().bind( Command::Argument::ResultColumn, types::Variant( resultsymbol), scope_functionidx);
	}

private:
	std::vector<Command> m_cmd;				//< list of commands of the transaction
};

typedef types::CountedReference<TransactionInput> TransactionInputR;

}}
#endif


