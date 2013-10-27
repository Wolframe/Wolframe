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
///\brief Internal interface for selector path of database transaction functions
///\file transactionfunction/Path.hpp
#ifndef _DATABASE_TRANSACTION_FUNCTION_PATH_HPP_INCLUDED
#define _DATABASE_TRANSACTION_FUNCTION_PATH_HPP_INCLUDED
#include "transactionFunctionDescription.hpp"
#include "transactionfunction/TagTable.hpp"
#include "transactionfunction/InputStructure.hpp"
#include <string>
#include <map>
#include <vector>

namespace _Wolframe {
namespace db {

class Path
{
public:
	typedef TransactionFunctionDescription::MainProcessingStep::Call Call;
	enum ElementType
	{
		Root,
		Next,
		Find,
		Up,
		ResultSymbol,
		ResultIndex,
		Constant
	};

	static const char* elementTypeName( ElementType i)
	{
		static const char* ar[] ={"Root","Next","Find","Up","ResultSymbol","ResultIndex","Constant"};
		return ar[(int)i];
	}

	struct Element
	{
		ElementType m_type;
		int m_tag;
		int m_scope_functionidx;

		Element()
			:m_type(Root),m_tag(0),m_scope_functionidx(-1){}
		Element( ElementType type_, int tag_, int scope_functionidx_)
			:m_type(type_),m_tag(tag_),m_scope_functionidx(scope_functionidx_){}
		Element( const Element& o)
			:m_type(o.m_type),m_tag(o.m_tag),m_scope_functionidx(o.m_scope_functionidx){}
	};

	Path(){}
	Path( const std::string& selector, TagTable* tagmap);
	Path( const Call::Param& param, const TransactionFunctionDescription::VariableTable* varmap, TagTable* tagmap);
	Path( const Path& o);
	std::string tostring() const;

	ElementType referenceType() const;
	std::size_t resultReferenceIndex() const;
	const std::string& resultReferenceSymbol() const;
	int resultReferenceScope() const;
	const std::string& constantReference() const;
	void selectNodes( const TransactionFunctionInput::Structure& st, const TransactionFunctionInput::Structure::Node* nd, std::vector<const TransactionFunctionInput::Structure::Node*>& ar) const;

	std::vector<Element>::const_iterator begin() const		{return m_path.begin();}
	std::vector<Element>::const_iterator end() const		{return m_path.end();}
	std::size_t size() const					{return m_path.size();}

	void rewrite( const std::map<int,int>& rwtab, int scope_functionidx_incr);
	void append( const Path& o);

private:
	void parseSelectorPath( const std::string& value, TagTable* tagmap);

private:
	friend class ConstantReferencePath;
	std::vector<Element> m_path;
	std::string m_content;
};

class ConstantReferencePath :public Path
{
public:
	ConstantReferencePath( const std::string& value);
};

}}//namespace
#endif
