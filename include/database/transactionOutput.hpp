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
///\brief Definition of transaction output
///\file database/transactionOutput.hpp
#ifndef _DATABASE_TRANSACTION_OUTPUT_HPP_INCLUDED
#define _DATABASE_TRANSACTION_OUTPUT_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include <string>
#include <vector>
#include <cstdlib>

namespace _Wolframe {
namespace db {

///\class TransactionOutput
///\brief Output of a transaction
class TransactionOutput
{
public:
	///\brief Constructor
	TransactionOutput()
		:m_isCaseSensitive(false){}
	///\brief Copy constructor
	TransactionOutput( const TransactionOutput& o)
		:m_result(o.m_result)
		,m_isCaseSensitive(o.m_isCaseSensitive){}

	///\class CommandResult
	///\brief Structure representing the result of a command in the transaction
	class CommandResult
	{
	public:
		typedef std::vector<types::Variant> Row;

	public:
		///\brief Constructor
		CommandResult( std::size_t functionidx_, std::size_t level_)
			:m_functionidx(functionidx_)
			,m_level(level_)
			,m_colidx(0){}

		///\brief Copy constructor
		CommandResult( const CommandResult& o)
			:m_functionidx(o.m_functionidx)
			,m_level(o.m_level)
			,m_columnName(o.m_columnName)
			,m_row(o.m_row)
			,m_colidx(o.m_colidx){}

		///\brief Copy by assigment
		CommandResult& operator=( const CommandResult& o)
		{
			m_functionidx = o.m_functionidx;
			m_level = o.m_level;
			m_columnName = o.m_columnName;
			m_row = o.m_row;
			m_colidx = o.m_colidx;
			return *this;
		}

		std::vector<Row>::const_iterator begin() const			{return m_row.begin();}
		std::vector<Row>::const_iterator end() const			{return m_row.end();}

		std::size_t functionidx() const					{return m_functionidx;}
		std::size_t level() const					{return m_level;}
		std::size_t nofRows() const					{return m_row.size();}
		std::size_t nofColumns() const					{return m_columnName.size();}
		const std::string& columnName( std::size_t i) const		{return m_columnName[i];}

	public://interface for constructing the result:
		void addColumn( const std::string& name)			{m_columnName.push_back( name);}
		void openRow()							{m_row.push_back( Row( nofColumns())); m_colidx=0;}
		void addValue( const types::Variant& v)				{m_row.back().at( m_colidx) = v; ++m_colidx;}
		void addNull()							{++m_colidx;}

	private:
		std::size_t m_functionidx;
		std::size_t m_level;
		std::vector<std::string> m_columnName;
		std::vector<Row> m_row;
		std::size_t m_colidx;
	};

public:// interface for accessing the result and iterating on the result:
	typedef std::vector<CommandResult>::const_iterator result_const_iterator;

	result_const_iterator begin() const					{return m_result.begin();}
	result_const_iterator end() const					{return m_result.end();}

	result_const_iterator last( std::size_t level) const
	{
		if (m_result.empty()) return end();
		result_const_iterator rt = m_result.begin() + m_result.size() -1;
		result_const_iterator begin_ = begin();
		while (rt->level() > level && rt != begin_) --rt;
		if (rt->level() != level) return end();
		return rt;
	}

	///\brief Get the size (number of elements) of the result
	std::size_t size() const						{return m_result.size();}

	///\brief Return the result as readable serialization for logging output
	std::string tostring() const;

	bool isCaseSensitive() const						{return m_isCaseSensitive;}
	void setCaseSensitive( bool v=true)					{m_isCaseSensitive = v;}

public:// interface for constructing the result:
	void openCommandResult( std::size_t functionidx, std::size_t level)	{m_result.push_back( CommandResult( functionidx, level));}
	void addCommandResult( const CommandResult& r)				{m_result.push_back( r);}
	void addColumn( const std::string& name)				{m_result.back().addColumn( name);}
	void openRow()								{m_result.back().openRow();}
	void addValue( const types::Variant& value_)				{m_result.back().addValue( value_);}
	void addNull()								{m_result.back().addNull();}

private:
	std::vector<CommandResult> m_result;					//< List of command results
	bool m_isCaseSensitive;							//< True if the element names in the result are case sensitive
};

typedef types::CountedReference<TransactionOutput> TransactionOutputR;

}}
#endif


