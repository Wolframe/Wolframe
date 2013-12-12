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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_DDL_FORM_HPP_INCLUDED
#define _Wolframe_DDL_FORM_HPP_INCLUDED
#include <string>
#include <vector>
#include <map>
#include <wchar.h>
#include "filter/typedfilter.hpp"

namespace _Wolframe {
namespace test {

class Form
{
public:
	enum Type
	{
		Atomic,
		Struct,
		Array
	};
	typedef std::map<std::string,Form> Map;
	typedef std::vector<Form> Vector;

public:
	explicit Form( const std::string& v)
		:m_type(Atomic),m_value(v){}
	Form( const Form& o)
		:m_type(o.m_type),m_map(o.m_map),m_ar(o.m_ar),m_value(o.m_value){}
	explicit Form( Type type_=Struct)
		:m_type(type_){}

	Form& operator()( const Form& value);
	Form& operator()( const std::string& name, const Form& value);

	const std::string& value() const;
	const wchar_t* wcvalue() const;
	langbind::TypedInputFilterR get() const;

	const Form& operator[]( const std::string& name) const;
	Form& operator[]( const std::string& name);
	const Form& operator[]( std::size_t idx) const;
	Form& operator[]( std::size_t idx);

	Map::const_iterator structbegin() const		{return m_map.begin();}
	Map::const_iterator structend() const		{return m_map.end();}
	Map::iterator structbegin()					{return m_map.begin();}
	Map::iterator structend()					{return m_map.end();}

	Vector::const_iterator arraybegin() const	{return m_ar.begin();}
	Vector::const_iterator arrayend() const		{return m_ar.end();}
	Vector::iterator arraybegin()				{return m_ar.begin();}
	Vector::iterator arrayend()					{return m_ar.end();}

	bool isAtomic() const						{return m_type == Atomic;}
	bool isArray() const						{return m_type == Array;}
	bool isStruct() const						{return m_type == Struct;}
	std::size_t size() const					{return (m_type == Atomic)?m_value.size():((m_type == Array)?m_ar.size():m_map.size());}
	bool empty() const							{return m_type == Struct && m_map.empty();}

private:
	Type m_type;
	Map m_map;
	Vector m_ar;
	std::string m_value;
};


class FormInputFilter
	:public langbind::TypedInputFilter
{
public:
	FormInputFilter( const Form* form);
	FormInputFilter( const FormInputFilter& o);
	virtual ~FormInputFilter(){}

	virtual FormInputFilter* copy() const	{return new FormInputFilter(*this);}

	virtual bool getNext( ElementType& type, types::VariantConst& element);

private:
	enum TagState
	{
		OpenTagState,
		CloseTagState,
		ValueState
	};

	struct StackElem
	{
		Form::Type type;
		std::string atag;
		Form::Vector::const_iterator aitr;
		Form::Vector::const_iterator aend;
		Form::Map::const_iterator sitr;
		Form::Map::const_iterator send;
		const std::string* ptr;
		TagState state;

		StackElem( const StackElem& o)
			:type(o.type),atag(o.atag),aitr(o.aitr),aend(o.aend),sitr(o.sitr),send(o.send),ptr(o.ptr),state(o.state){}
		StackElem( const Form::Map::const_iterator& itr_, const Form::Map::const_iterator& end_)
			:type(Form::Struct),sitr(itr_),send(end_),ptr(0),state(OpenTagState){}
		StackElem( const std::string& atag_, const Form::Vector::const_iterator& itr_, const Form::Vector::const_iterator& end_)
			:type(Form::Array),atag(atag_),aitr(itr_),aend(end_),ptr(0),state(ValueState){}
		StackElem( const std::string* ptr_)
			:type(Form::Atomic),ptr(ptr_),state(ValueState){}

		bool end() const;
	};

	std::vector<StackElem> m_stk;
};

}}//namespace
#endif
