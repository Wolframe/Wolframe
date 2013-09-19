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
///\brief Internal interface for output data structure of transaction function
///\file transactionfunction/OutputResultStructure.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_OUTPUT_RESULT_STRUCTURE_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_OUTPUT_RESULT_STRUCTURE_HPP_INCLUDED
#include "database/transactionFunction.hpp"
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace db {

class ResultElement
{
public:
	enum Type {IgnoreResult,OpenTag,CloseTag,Value,Constant,FunctionStart,FunctionEnd,IndexStart,IndexEnd,OperationStart,OperationEnd,SelectResultFunction,SelectResultColumn,SelectResultColumnName};
	static const char* typeName( Type i)
	{
		static const char* ar[] = {"IgnoreResult","OpenTag","CloseTag","Value","Constant","FunctionStart","FunctionEnd","IndexStart","IndexEnd","OperationStart","OperationEnd","SelectResultFunction","SelectResultColumn","SelectResultColumnName"};
		return ar[ (int)i];
	}

	ResultElement()
		:m_type(CloseTag),m_idx(0){}
	ResultElement( Type type_, std::size_t idx_)
		:m_type(type_),m_idx(idx_){}
	ResultElement( const ResultElement& o)
		:m_type(o.m_type),m_idx(o.m_idx){}

	Type type() const		{return m_type;}
	std::size_t idx() const		{return m_idx;}

private:
	Type m_type;
	std::size_t m_idx;
};

class ResultStructure
{
public:
	ResultStructure();
	ResultStructure( const ResultStructure& o);

	struct ContentElement
	{
		const char* value;
		ResultElement::Type type;
		int idx;

		ContentElement();
		ContentElement( const ContentElement& o);
	};

	class const_iterator
	{
	public:
		explicit const_iterator( const ResultStructure* struct_=0);
		const_iterator( const const_iterator& o);

		bool operator==(const const_iterator& o) const;
		bool operator!=(const const_iterator& o) const	{return !operator==(o);}

		const_iterator& operator++()			{if (m_struct) {++m_itr; init();} return *this;}
		const_iterator operator++(int)			{const_iterator rt=*this; ++(*this); return rt;}

		const ContentElement* operator->() const	{return &m_content;}
		const ContentElement& operator*() const		{return m_content;}

	private:
		void init();

	private:
		ContentElement m_content;
		std::vector<ResultElement>::const_iterator m_itr;
		const ResultStructure* m_struct;
	};

public:
	const_iterator begin() const	{return const_iterator(this);}
	const_iterator end() const	{return const_iterator();}

	void addOpenTag( const std::string& name);
	void addResultColumnName( const std::string& name);
	void addConstant( const std::string& value);
	void addCloseTag();

	void addIgnoreResult( std::size_t functionidx);
	void addValueReference( std::size_t functionidx);
	void addMark( ResultElement::Type mrk, std::size_t functionidx);
	void addEmbeddedResult( const ResultStructure& o, std::size_t functionidx);
	std::string tostring() const;
	std::size_t size() const	{return m_ar.size();}

private:
	std::vector<ResultElement> m_ar;
	std::string m_strings;
};

typedef types::CountedReference<ResultStructure> ResultStructureR;

}}//namespace
#endif

